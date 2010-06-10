Description
-----------

This is the HTML Tidy library built as a Ruby extension.

The original C source for Tidy can be found at http://tidy.sourceforge.net/

This version of Ruby Tidy is compatible enough with the other to be called by Tarantula (http://github.com/relevance/tarantula).

Usage
-----

<pre>
require 'tidy'
require 'open-uri'

tidy = Tidy.new

uri = open("http://www.google.com")
errors, html = tidy.parse(uri.read)

puts errors
</pre>

or, for compatibility with the other Tidy library:

<pre>
  require 'tidy'

  tidy = Tidy.open({}) do |tidy|
    xml = tidy.clean("<html><body>String</body></html>")
  end
</pre>

If you want to run Tarantula on your Rails application, you may want to create a rake task lib/tasks/tarantula.rake that looks like:

<pre>
  ENV["RAILS_ENV"] = "test" # so that rake tarantula:test always runs in your test env
  ENV["TIDY_PATH"] = "tidy" # Tarantula will not load Tidy if this is not set

  load File.join(RAILS_ROOT, Dir["vendor/gems/tarantula-*/tasks/*.rake"])
</pre>

Installing
----------

<pre>
$ [sudo] gem install tidy-ext
</pre>


