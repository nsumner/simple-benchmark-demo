#!/usr/bin/env python3

from collections import defaultdict
from copy import deepcopy
import datetime
import json

import matplotlib as mpl
mpl.rcParams.update({
    "text.usetex": "true",
    "font.family": "serif",
    "font.weight": "heavy",
    "font.size": "16",
    "font.serif": [],
    "font.sans-serif": [],
})

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


SYMBOLS = [ 'o', 'v', 's', '*', 'D', '+', 'x', '^', '8']


def split_name(name):
    template_start = name.index('<')
    template_end = name.rindex('>')
    size_start = name.rindex('/')
    return (name[:template_start],
            name[template_start+1:template_end],
            name[size_start+1:])


def group_results_by_operation(raw_results):
    results = defaultdict(lambda : defaultdict(list))
    for result in raw_results:
        try:
            test_name, structure, size = split_name(result['name'])
            results[test_name][structure].append((size, result['cpu_time']))
        except:
            print('Skipping', result['name'])
    return results


def clean_adt_name(adt_name):
    if adt_name.startswith('std::'):
        adt_name = adt_name[5:]
    adt_name = str.replace(adt_name, '<', '$<$')
    adt_name = str.replace(adt_name, '>', '$>$')
    return adt_name


def extract_num_ops(test_results):
    numOps = [s[0] for s in test_results[0][1]]
    return [1024*int(ops[:-1])      if ops[-1:] == 'k' else
            1024*1024*int(ops[:-1]) if ops[-1:] == 'M' else
            int(ops) for ops in numOps]


def plot_one_test(name, results, pdf):
    plt.figure()
    numOps = extract_num_ops(results) if results else []

    for count, (series_name, series) in enumerate(results):
        times = [s[1] for s in series]
        plt.plot(numOps, times,
                 marker=SYMBOLS[count][:len(times)],
                 fillstyle='full',
                 markersize=10,
                 linewidth=2,
                 label=clean_adt_name(series_name))

    plt.xlabel('Matrix Dimension')
    plt.ylabel('CPU Time (ns)')
    plt.title(clean_adt_name(name))
    plt.legend(loc='upper left')
    plt.tight_layout()
    pdf.savefig()
    plt.close()


def plot_results(results):
    with PdfPages('matrixPerformance.pdf') as pdf:
        for test_name, test_results in results:
            plot_one_test(test_name, test_results, pdf)
        d = pdf.infodict()
        d['Title'] = 'Performance Comparison of Square Matrix Traversal'
        d['Author'] = 'Nick Sumner'
        d['CreationDate'] = d['ModDate'] = datetime.datetime.today()


def satisfies_filter(struct_name, test_name, keep, skip):
    return ((skip is None
                or all(struct_name.find(skipped) < 0 for skipped in skip)
                    and all(test_name.find(skipped) < 0 for skipped in skip))
           and (keep is None
               or any(struct_name.find(kept) >= 0 for kept in keep)
               or any(test_name.find(kept) >= 0 for kept in keep)))


def get_series_key(series):
    return tuple(reversed(series[0].lower().split('<')))


def get_parameter_name(struct_name):
    template_start = struct_name.index('<')
    template_end   = struct_name.rindex('>')
    return struct_name[template_start + 1 : template_end]


def get_configuration_label(test, adt, query):
    return test + ('-' + get_parameter_name(adt) if '<' not in query else '')


def results_for_test(result_map, test_name,
                     keep=None, skip=None, prefix=None, xrange=None,
                     scale=lambda x, y: y):
    results = []
    prefix = prefix if prefix else ''
    for task_name, task_results in result_map[test_name].items():
        if not satisfies_filter(task_name, test_name, keep, skip):
            continue
        task_results = [(size, scale(size, timing))
                        for size, timing in task_results
                        if not xrange or (xrange[0] < int(size) <= xrange[1])]
        task_results.sort(key=lambda x: int(x[0]))
        results.append((prefix + task_name, task_results))
    results.sort(key=get_series_key)
    return results


if __name__ == '__main__':
    import sys
    with open(sys.argv[1]) as infile:
        raw_results = json.loads(infile.read())['benchmarks']
    result_map = group_results_by_operation(raw_results)
    reports = (
         ('Friendly Order',
          results_for_test(result_map,'testAccess', keep=['Friendly'], skip=['ReadD'])),
         ('Friendly Order',
          results_for_test(result_map,'testAccess', keep=['Friendly'])),
         ('Unfriendly Order',
          results_for_test(result_map,'testAccess', keep=['Unfriendly'])),
         ('Both Orders',
          results_for_test(result_map,'testAccess')),
         ('Both Orders',
          results_for_test(result_map,'testAccess', xrange=(0,33000))),
         ('Both Orders',
          results_for_test(result_map,'testAccess', xrange=(0,8200))),
         ('Both Orders',
          results_for_test(result_map,'testAccess', xrange=(0,4100))),
         ('Both Orders',
          results_for_test(result_map,'testAccess', xrange=(0,1050))),
         ('Friendly Order 1/$n^2$ scaled',
          results_for_test(result_map,'testAccess',
                           scale=lambda x, y: y / (int(x)**2),
                           keep=['Friendly'])),
         ('Friendly Order 1/$n^2$ scaled',
          results_for_test(result_map,'testAccess',
                           scale=lambda x, y: y / (int(x)**2),
                           keep=['Friendly'], skip=['Dependent'])),
         ('Both Orders 1/$n^2$ scaled',
          results_for_test(result_map,'testAccess', scale=lambda x, y: y / (int(x)**2))),
    )

    plot_results(reports)
