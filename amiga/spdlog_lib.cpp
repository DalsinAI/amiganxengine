// spdlog 1.17.0 and its bundled fmt 12.1.0 (NXEngine-evo's deps/spdlog) as a
// compiled library, as spdlog's own src/*.cpp build it: every other source
// is compiled with SPDLOG_COMPILED_LIB, so the logger's implementation and
// fmt's are compiled here once instead of in each of the 131 sources (each
// took ten times as long with them; stove_build has 900 s). The same
// includes and explicit instantiations as spdlog's src/spdlog.cpp,
// src/stdout_sinks.cpp, src/color_sinks.cpp, src/file_sinks.cpp and
// src/bundled_fmtlib_format.cpp (fmt's src/format.cc), for the sinks
// NXEngine-evo uses.
// MIT, Copyright (c) 2026 AmiCradle Team (spdlog and fmt: MIT, their
// authors).
#ifndef SPDLOG_COMPILED_LIB
#error define SPDLOG_COMPILED_LIB
#endif

// src/bundled_fmtlib_format.cpp (fmt's src/format.cc)
#include <spdlog/fmt/bundled/format-inl.h>

FMT_BEGIN_NAMESPACE
#if FMT_USE_LOCALE
template FMT_API locale_ref::locale_ref(const std::locale &loc);
template FMT_API auto locale_ref::get<std::locale>() const -> std::locale;
#endif
namespace detail
{
template FMT_API auto dragonbox::to_decimal(float x) noexcept -> dragonbox::decimal_fp<float>;
template FMT_API auto dragonbox::to_decimal(double x) noexcept -> dragonbox::decimal_fp<double>;
template FMT_API auto thousands_sep_impl(locale_ref) -> thousands_sep_result<char>;
template FMT_API auto decimal_point_impl(locale_ref) -> char;
template FMT_API void buffer<char>::append(const char *, const char *);
template FMT_API auto thousands_sep_impl(locale_ref) -> thousands_sep_result<wchar_t>;
template FMT_API auto decimal_point_impl(locale_ref) -> wchar_t;
template FMT_API void buffer<wchar_t>::append(const wchar_t *, const wchar_t *);
} // namespace detail
FMT_END_NAMESPACE

// src/spdlog.cpp (the registry's flusher, periodic_worker, with it)
#include <spdlog/common-inl.h>
#include <spdlog/details/backtracer-inl.h>
#include <spdlog/details/log_msg-inl.h>
#include <spdlog/details/log_msg_buffer-inl.h>
#include <spdlog/details/os-inl.h>
#include <spdlog/details/periodic_worker-inl.h>
#include <spdlog/details/registry-inl.h>
#include <spdlog/logger-inl.h>
#include <spdlog/pattern_formatter-inl.h>
#include <spdlog/sinks/base_sink-inl.h>
#include <spdlog/sinks/sink-inl.h>
#include <spdlog/spdlog-inl.h>

#include <mutex>

template SPDLOG_API spdlog::logger::logger(std::string name, sinks_init_list::iterator begin,
                                           sinks_init_list::iterator end);
template class SPDLOG_API spdlog::sinks::base_sink<std::mutex>;
template class SPDLOG_API spdlog::sinks::base_sink<spdlog::details::null_mutex>;

// src/stdout_sinks.cpp and src/color_sinks.cpp (not Windows)
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/stdout_sinks-inl.h>
#include <spdlog/sinks/ansicolor_sink-inl.h>
#include <spdlog/sinks/stdout_color_sinks-inl.h>

template class SPDLOG_API spdlog::sinks::stdout_sink_base<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::stdout_sink_base<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::stdout_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::stdout_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::stderr_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::stderr_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stderr_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stderr_sink<spdlog::details::console_nullmutex>;

// src/file_sinks.cpp (the basic file sink)
#include <spdlog/details/file_helper-inl.h>
#include <spdlog/sinks/basic_file_sink-inl.h>

template class SPDLOG_API spdlog::sinks::basic_file_sink<std::mutex>;
template class SPDLOG_API spdlog::sinks::basic_file_sink<spdlog::details::null_mutex>;
