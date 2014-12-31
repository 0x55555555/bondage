require_relative 'Copyable'
require_relative 'Basic'
require_relative 'Default'

module CPP

DefaultClassTypes = {
  :copyable => ClassTypes::Copyable.new,
  :managed => ClassTypes::Basic.new("MANAGED"),
  :unmanaged => ClassTypes::Basic.new("UNMANAGED"),
  :sharedpointer => ClassTypes::Basic.new("SHARED_POINTER"),
  :weakpointer => ClassTypes::Basic.new("WEAK_POINTER"),
  :uniquepointer => ClassTypes::Basic.new("UNIQUE_POINTER"),
  :default => ClassTypes::Default.new
}

end