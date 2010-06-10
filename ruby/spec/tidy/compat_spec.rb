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

    tidy = Tidy.open({:show_warnings => false}) do |tidy|
      html = tidy.clean("<html><body>String</body></html>")
      errors1 = tidy.errors
      puts errors1
    end

    tidy = Tidy.open({:show_warnings => true}) do |tidy|
      html = tidy.clean("<html><body>String</body></html>")
      errors2 = tidy.errors
      puts errors2
    end

    errors1.should_not == errors2
  end
end
