#!/usr/bin/python3.9
"""Basic CGI script that prints the submitted data.

Note: when possible, use a framework (e.g Flask) instead of writing CGI scripts.
"""

import cgi
import sys

def template():
    """Returns the template with keyword arguments to be formatted.

    Note: It is a good idea to move this to an HTML file."""
    templ = """
<html>
<title>{title}</title>
<body>
  <h3>Username: {username}</h3>
  <h3>Password: {password}</h3>
</body>
</html>
            """
    return templ


def render(**kwargs):
    content = template().format(**kwargs)
    print(content)


def run():
    form = cgi.FieldStorage()
    username = form.getfirst('username', '')
    password = form.getfirst('password', '')
    render(title='Add a User', **locals())


if __name__ == '__main__':
    try:
        # Tell the browser it's an HTML page.
        print('Content-Type: text/HTML')
        # Blank line to indicate end of headers.
        print('')
        # Main program.
        run()
    except:
        cgi.print_exception()
