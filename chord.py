"""Simple web server to show chord diagrams"""

__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"

import bottle


@bottle.route('/data/<filename:path>')
def data(filename):
    """Renders chord diagram for a specific field.

    Args:
        filename (string): The filename in data to load
    """
    return bottle.static_file(filename, root='./../data/reduced/')


@bottle.route('/static/<static_path:path>')
def load_file(static_path):
    """Renders chord diagram for a specific field.

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
    return bottle.template('chord', field=field, weightedness=weightedness)


if __name__  == '__main__':
    bottle.TEMPLATE_PATH = ['.']
    bottle.run(host='localhost', port=8080)
