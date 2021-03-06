# Copyright (C) 2015  Ruby-GNOME2 Project Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

class TestGLibBinding < Test::Unit::TestCase
  include GLibTestUtils

  class DataObject < GLib::Object
    type_register

    install_property(GLib::Param::Int.new("source", # name
                                          "Source", # nick
                                          "The source data", # blurb
                                          0,     # min
                                          100,   # max
                                          0,     # default
                                          GLib::Param::READABLE |
                                          GLib::Param::WRITABLE))
    install_property(GLib::Param::Int.new("target", # name
                                          "Target", # nick
                                          "The target data", # blurb
                                          0,     # min
                                          100,   # max
                                          0,     # default
                                          GLib::Param::READABLE |
                                          GLib::Param::WRITABLE))

    attr_reader :source, :target
    def initialize
      @source = 0
      @target = 0
      super
    end

    def source=(value)
      @source = value
      notify("source")
    end

    def target=(value)
      @target = value
      notify("target")
    end
  end

  setup do
    only_glib_version(2, 26, 0)
  end

  setup do
    @source = DataObject.new
    @target = DataObject.new
    @binding = @source.bind_property("source", @target, "target", :default)
  end

  test "#source" do
    assert_equal(@source, @binding.source)
  end

  test "#source_property" do
    assert_equal("source", @binding.source_property)
  end

  test "#target" do
    assert_equal(@target, @binding.target)
  end

  test "#target_property" do
    assert_equal("target", @binding.target_property)
  end

  test "#flags" do
    assert_equal(GLib::BindingFlags::DEFAULT, @binding.flags)
  end

  test "#unbind" do
    only_glib_version(2, 38, 0)
    assert_equal(0, @target.target)
    @source.source = 10
    assert_equal(10, @target.target)
    @binding.unbind
    @source.source = 20
    assert_equal(10, @target.target)
  end
end
