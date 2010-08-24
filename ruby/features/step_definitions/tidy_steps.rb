require 'tidy'

Given /^(.+) is tidy$/ do |page_name|

  visit path_to(page_name)

  tidy = Tidy.open({:show_warnings => true}) do |tidy|
    out = tidy.clean(response.body)
  end

  tidy.errors.scan(/(\d+) warnings?, (\d+) errors? were found!/) do |w,e|
    warnings = w.to_i
    errors   = e.to_i
    unless warnings == 0 && errors == 0
      raise tidy.errors
    end
  end

end

