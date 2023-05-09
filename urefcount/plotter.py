#!/usr/bin/env python3

from optparse import OptionParser
import pandas as pd
import matplotlib.pyplot as plt


def main():
    parser = OptionParser()
    parser.add_option('-l', '--log',
                      dest='log',
                      help='Log file from the urefcount benchmark (.csv)',
                      default=None)
    parser.add_option('-o', '--output',
                      dest='output',
                      help='Saved name after plotting',
                      default='result.png')
    (options, _) = parser.parse_args()
    df = pd.read_csv(options.log)
    for type, df_type in df.groupby('type'):
        type = type[len('virtual_file_'):]
        if len(type) == 0:
            type = 'base'
        X = df_type['N']
        Y = df_type['iters']
        plt.plot(X, Y, label=type)
    plt.xlabel('#Cores')
    plt.ylabel('#Reads')
    plt.legend()
    plt.savefig(options.output)


if __name__ == "__main__":
    main()
