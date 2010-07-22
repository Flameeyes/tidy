require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy compatibility methods" do

  subject { Tidy.new }

  it "should be defined" do
    defined?(Tidy).should be_true
  end

  it "should have an open method" do
    Tidy.should respond_to :open
  end

  it "should have a clean method" do
    subject.should respond_to :clean
  end

  it "should have an errors attribute" do
    subject.should respond_to :errors
  end

  it "should have a path attribute" do
    Tidy.should respond_to :path
    Tidy.path = "hello world"
    Tidy.path.should == "hello world"
  end

  it "should parse a string" do
    tidy = Tidy.open({}) do |tidy|
      html = tidy.clean("<html><body>String</body></html>")
      html.should_not be_empty
    end
  end

  it "should be able to parse more than one string consecutively" do
    tidy = Tidy.open({}) do |tidy|
      html1 = tidy.clean("<html><body>String</body></html>")
      errors1 = tidy.errors
      html2 = tidy.clean("<html><body>String</body></html>")
      errors2 = tidy.errors

      errors1.should == errors2
    end
  end

  it "should observe the show_warnings entry in the option hash" do
    errors1 = nil
    errors2 = nil

    tidy = Tidy.open({'show-warnings' => false}) do |tidy|
      html = tidy.clean("<html><body>String</body></html>")
      errors1 = tidy.errors
    end

    tidy = Tidy.open({'show-warnings' => true}) do |tidy|
      html = tidy.clean("<html><body>String</body></html>")
      errors2 = tidy.errors
    end

    errors1.should_not == errors2
  end

  it "should observe the options hash when it uses strings" do
    laundry = "<html><body>String<img src=''/></body></html>"
    options = {
      'alt-text' => 'hello world',
      'uppercase-tags' => true
    }
    tidy = Tidy.open(options) do |tidy|
      clean = tidy.clean(laundry)
      clean.should =~ /<HTML>/
      clean.should =~ /alt="hello world"/
    end
  end

  it "should observe the options hash when it uses symbols" do
    laundry = "<html><body>String<img src=''/></body></html>"
    options = {
      :alt_text => 'hello world',
      :uppercase_tags => true
    }
    tidy = Tidy.open(options) do |tidy|
      clean = tidy.clean(laundry)
      clean.should =~ /<HTML>/
      clean.should =~ /alt="hello world"/
    end
  end
end
