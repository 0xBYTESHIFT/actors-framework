#pragma once

#include <actors-framework/config.hpp>

#include <string_view>

namespace actors_framework::detail {

    using std::basic_string_view;
    using std::string_view;
    using std::u16string_view;
    using std::u32string_view;
    using std::wstring_view;

    using std::operator==;
    using std::operator!=;
    using std::operator<;
    using std::operator<=;
    using std::operator>;
    using std::operator>=;

    using std::operator<<;

} // namespace actors_framework::detail
