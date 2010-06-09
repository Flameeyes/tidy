Description
-----------

This is the HTML Tidy library built as a Ruby extension.

The original C source for Tidy can be found at http://tidy.sourceforge.net/

Usage
-----

<pre>
require 'tidy'
require 'open-uri'

tidy = Tidy.new
html = open("http://www.google.com")
errors, html = tidy.parse(html.read)

puts errors
</pre>

Installing
----------

<pre>
$ [sudo] gem install tidy
</pre>


