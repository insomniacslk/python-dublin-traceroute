from __future__ import absolute_import
from __future__ import print_function

import sys
import copy
import json
import datetime

import tabulate


class TracerouteResults(dict):

    def __init__(self, json_results, min_ttl=1):
        dict.__init__(self, json_results)
        self._flattened = None
        self.min_ttl = min_ttl

    def save(self, filename):
        with open(filename, 'w') as fd:
            json.dump(self, fd)

    def flatten(self, rebuild=False):
        '''
        Flatten the traceroute JSON data and return them as a list of {k: v}
        rows. NOTE: the flattened object is cached, and it can be modified by
        other code. If you need an independent copy, call with rebuild=True .
        '''
        if not rebuild and self._flattened is not None:
            return self._flattened

        rows = []
        results = copy.deepcopy(self)
        for port, flow in results['flows'].items():
            for packet in flow:

                sent = packet['sent']
                del packet['sent']

                packet['sent_timestamp'] = sent['timestamp']
                for k, v in sent['ip'].items():
                    packet['sent_ip_{k}'.format(k=k)] = v
                for k, v in sent['udp'].items():
                    packet['sent_udp_{k}'.format(k=k)] = v

                received = packet['received']
                del packet['received']
                if received:
                    packet['received_timestamp'] = received['timestamp']
                    try:
                        for k, v in received['ip'].items():
                            packet['received_ip_{k}'.format(k=k)] = v
                    except KeyError:
                        pass
                    try:
                        for k, v in received['icmp'].items():
                            packet['received_icmp_{k}'.format(k=k)] = v
                    except KeyError:
                        pass

                rows.append(packet)
        self._flattened = rows
        return rows

    def to_dataframe(self):
        '''
        Convert traceroute results to a Pandas DataFrame.
        '''
        # pandas is imported late because it does not compile yet on PyPy
        import pandas
        return pandas.DataFrame(self.flatten())

    def pretty_print(self, file=sys.stdout):
        '''
        Print the traceroute results in a tabular form.
        '''
        headers = ['ttl'] + list(self['flows'].keys())
        columns = []
        max_hops = 0
        for flow_id, hops in self['flows'].items():
            column = []
            for hop in hops:
                try:
                    if hop['received']['ip']['src'] != hop['name']:
                        name = '\n' + hop['name']
                    else:
                        name = hop['received']['ip']['src']
                    column.append('{name} ({rtt} usec)'.format(
                        name=name,
                        rtt=hop['rtt_usec'])
                    )
                except TypeError:
                    column.append('*')
                if hop['is_last']:
                    break
            max_hops = max(max_hops, len(column))
            columns.append(column)
        columns = [range(self.min_ttl, self.min_ttl + max_hops)] + columns
        rows = zip(*columns)
        print(tabulate.tabulate(rows, headers=headers), file=file)

    def stats(self, file=sys.stdout):
        df = self.to_dataframe()
        start_ts = df.sent_timestamp.astype(float).min()
        start_time = datetime.datetime.fromtimestamp(start_ts)
        end_ts = df.received_timestamp.astype(float).max()
        end_time = datetime.datetime.fromtimestamp(end_ts)
        total_time = end_time - start_time
        group = df.groupby(['sent_udp_sport', 'sent_udp_dport'])
        num_flows = len(group.all())
        num_distinct_flows = len(
            set([tuple(s[1].name) for s in group]))
        max_ttl = df[df.is_last == True].sent_ip_ttl.dropna().max()
        _nat_lengths = [
            len(s[1].nat_id.dropna().drop_duplicates()) for s in group]
        _visited_hosts = df.received_ip_src.dropna()
        total_responding_hosts = len(_visited_hosts)
        total_distinct_responding_hosts = len(_visited_hosts.drop_duplicates())
        print(
            'Start time                      : {st}\n'
            'End time                        : {et}\n'
            'Total time                      : {tt}\n'
            'Number of probed net flows      : {nnf}\n'
            'Number of distinct net flows    : {ndnf}\n'
            'Max TTL reached                 : {mttl}\n'
            'Min traversed NATs              : {mtn}\n'
            'Max traversed NATs              : {mmtn}\n'
            'Total responding hosts          : {trh}\n'
            'Total distinct responding hosts : {tdrh}\n'
            .format(
                st=start_time,
                et=end_time,
                tt=total_time,
                nnf=num_flows,
                ndnf=num_distinct_flows,
                mttl=max_ttl,
                mtn=min(_nat_lengths),
                mmtn=max(_nat_lengths),
                trh=total_responding_hosts,
                tdrh=total_distinct_responding_hosts,
             ), file=file)
