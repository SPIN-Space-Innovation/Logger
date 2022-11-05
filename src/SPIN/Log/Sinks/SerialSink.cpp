/**
 * MIT License
 *
 * Copyright (c) 2022 SPIN - Space Innovation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include <SPIN/Log/Sinks/SerialSink.hpp>



#ifdef ARDUINO
    #include <Arduino.h>
    #include <stdint.h>
#else
    #include <ostream>
    #include <cstdint>
#endif

static const char uncolouredTag[6][7] = {
    "[VER]:",
    "[DEB]:",
    "[INF]:",
    "[WAR]:",
    "[ERR]:",
    "[FAT]:"
};
static const char colouredTag[6][16] = {
    "[\e[37mVER\e[0m]:",
    "[\e[36mDEB\e[0m]:",
    "[\e[93mINF\e[0m]:",
    "[\e[34mWAR\e[0m]:",
    "[\e[31mERR\e[0m]:",
    "[\e[91mFAT\e[0m]:"
};



#ifdef ARDUINO
SPIN::Log::Sinks::SerialSink::SerialSink(Stream* stream, bool coloured)
#else
SPIN::Log::Sinks::SerialSink::SerialSink(std::ostream* stream, bool coloured)
#endif
{
    this->_stream = stream;
    this->_coloured = coloured;
}
SPIN::Log::Sinks::SerialSink::SerialSink(const SPIN::Log::Sinks::SerialSink& obj)
{
    this->_stream = obj._stream;
    this->_coloured = obj._coloured;
}
SPIN::Log::Sinks::SerialSink::SerialSink(SPIN::Log::Sinks::SerialSink&& deadObj) noexcept {
    this->_stream = deadObj._stream;
    this->_coloured = deadObj._coloured;

    deadObj._stream = nullptr;
}

void SPIN::Log::Sinks::SerialSink::Handle(SPIN::Log::LogLevel logLevel, const char* message)
{
    if (this->_stream == nullptr)
    {
        return;
    }

    const char* tag = (this->_coloured) ? colouredTag[(uint8_t)logLevel] : uncolouredTag[(uint8_t)logLevel];

#ifdef ARDUINO
    this->_stream->print(tag);
    this->_stream->print(' ');
    this->_stream->println(message);
#else
    *(this->_stream) << tag << ' ' << message << '\n';
#endif
}
void SPIN::Log::Sinks::SerialSink::Flush()
{
#ifdef ARDUINO
    this->_stream->flush();
#else
    *(this->_stream) << std::flush;
#endif
}


SPIN::Log::Sinks::SerialSink& SPIN::Log::Sinks::SerialSink::operator=(const SPIN::Log::Sinks::SerialSink& obj) = default;
SPIN::Log::Sinks::SerialSink& SPIN::Log::Sinks::SerialSink::operator=(SPIN::Log::Sinks::SerialSink&& deadObj) noexcept {
    this->_stream = deadObj._stream;
    this->_coloured = deadObj._coloured;

    deadObj._stream = nullptr;

    return *this;
}



SPIN::Log::Sinks::Factory::SerialSinkFactory::SerialSinkFactory() = default;
SPIN::Log::Sinks::Factory::SerialSinkFactory::SerialSinkFactory(const SPIN::Log::Sinks::Factory::SerialSinkFactory& obj)
{
    this->_stream = obj._stream;
    this->_coloured = obj._coloured;
}
SPIN::Log::Sinks::Factory::SerialSinkFactory::SerialSinkFactory(SPIN::Log::Sinks::Factory::SerialSinkFactory&& deadObj) noexcept {
    this->_stream = deadObj._stream;
    this->_coloured = deadObj._coloured;

    deadObj._stream = nullptr;
}


#ifdef ARDUINO
SPIN::Log::Sinks::Factory::SerialSinkFactory& SPIN::Log::Sinks::Factory::SerialSinkFactory::SetStream(Stream* stream)
#else
SPIN::Log::Sinks::Factory::SerialSinkFactory& SPIN::Log::Sinks::Factory::SerialSinkFactory::SetStream(std::ostream* stream)
#endif
{
    this->_stream = stream;

    return *this;
}
SPIN::Log::Sinks::Factory::SerialSinkFactory& SPIN::Log::Sinks::Factory::SerialSinkFactory::SetColoured(bool coloured)
{
    this->_coloured = coloured;

    return *this;
}


SPIN::Log::Sinks::SerialSink SPIN::Log::Sinks::Factory::SerialSinkFactory::Build()
{
    auto sink = SPIN::Log::Sinks::SerialSink(this->_stream, this->_coloured);

    return sink;
}


SPIN::Log::Sinks::Factory::SerialSinkFactory& SPIN::Log::Sinks::Factory::SerialSinkFactory::operator=(const SPIN::Log::Sinks::Factory::SerialSinkFactory& obj) = default;
SPIN::Log::Sinks::Factory::SerialSinkFactory& SPIN::Log::Sinks::Factory::SerialSinkFactory::operator=(SPIN::Log::Sinks::Factory::SerialSinkFactory&& deadObj) noexcept {
    this->_stream = deadObj._stream;
    this->_coloured = deadObj._coloured;

    deadObj._stream = nullptr;

    return *this;
}