"""Simple web server to show chord diagrams"""

__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import bottle
import glob
import json
import numpy

import edge_analysis

def render_network_to_json(network, field):
    """Rends (vertex1, vertex2) => edge style network to json.

    Args:
        network ((string, string) => num): The network to render.
        field (string): The name of the field.

    Returns:
        A json string containing the network
    """
    field1 = ''.join([field, '1'])
    field2 = ''.join([field, '2'])
    rows = []
    for vertex1, vertex2, edge in network:
        rows.append({
            field1: vertex1.replace("'", ''),
            field2: vertex2.replace("'", ''),
            'count': edge })

    return json.dumps(rows)


@bottle.route('/static/<static_path:path>')
def load_file(static_path):
    """Loads a static resource.

    Args:
        static_path (string): The file path relative to the current directory.
    """
    return bottle.static_file(static_path, root='./')


@bottle.route('/<field>/<weightedness>')
def index(field, weightedness):
    """Renders chord diagram for a specific field.

    Args:
        field (string): A field, e.g. "major1"
        weightedness (string): Either weighted, unweighted, or weighted_norm
    """
    network_filename = '../data/reduced/network_{}_{}.tsv'.format(
            field, weightedness)
    try:
        with open(network_filename, 'r') as network_file:
            full_network = list(edge_analysis.get_vertices_values(network_file))
    except FileNotFoundError:
        bottle.abort(404, 'Couldn\'t find correct data file!')

    rendered_network = render_network_to_json(full_network, field)
    return bottle.template('chord', field=field, data=rendered_network)


if __name__  == '__main__':
    bottle.TEMPLATE_PATH = ['.']
    bottle.run(host='localhost', port=8080)
