#pragma once
#include "ruby.h"
#include "bondage/Library.h"

void buildRubyModule(VALUE &rbLib, const bondage::Library &lib);

