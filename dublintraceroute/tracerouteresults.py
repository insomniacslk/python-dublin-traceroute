import pandas
import tabulate


class TracerouteResults(dict):

    def __init__(self, json_results):
        dict.__init__(self, json_results)

    def flatten(self):
        '''
        Flatten the traceroute JSON data and return them as a list of {k: v}
        rows
        '''
        rows = []
        for port, flow in self['flows'].items():
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
        return rows

    def to_dataframe(self):
        return pandas.DataFrame(self.flatten())

    def pretty_print(self):
        '''
        Print the traceroute results in a tabular form.
        '''
        # tabulate is imported here so it's not a requirement at module load
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
        columns = [range(1, max_hops + 1)] + columns
        rows = zip(*columns)
        print(tabulate.tabulate(rows, headers=headers))
