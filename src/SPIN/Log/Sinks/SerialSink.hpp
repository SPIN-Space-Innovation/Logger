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

#if !defined(__LOGGER__SPIN__LOG__SINKS_SERIALSINK__H__) && defined(__cplusplus)
#define __LOGGER__SPIN__LOG__SINKS_SERIALSINK__H__

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <ostream>
#endif

#include <SPIN/Log/LogLevel.hpp>
#include <SPIN/Log/Sinks/ISink.hpp>

namespace SPIN
{
    namespace Log
    {
        namespace Sinks
        {
            namespace Factory
            {
                class SerialSinkFactory;
            }

            class SerialSink : public SPIN::Log::Sinks::ISink
            {
                private:
#ifdef ARDUINO
                    Stream*
#else
                    std::ostream*
#endif
                        _stream = nullptr;
                    bool _coloured = false;

#ifdef ARDUINO
                    SerialSink(Stream*, bool);
#else
                    SerialSink(std::ostream*, bool);
#endif

                    friend class SPIN::Log::Sinks::Factory::SerialSinkFactory;

                public:
                    SerialSink() = delete;
                    SerialSink(const SerialSink&);
                    SerialSink(SerialSink&&) noexcept;

                    void Handle(SPIN::Log::LogLevel, const char*) override;
                    void Flush() override;

                    SerialSink& operator=(const SerialSink&);
                    SerialSink& operator=(SerialSink&&) noexcept;
            };

            namespace Factory
            {
                class SerialSinkFactory
                {
                    private:
#ifdef ARDUINO
                        Stream*
#else
                        std::ostream*
#endif
                            _stream = nullptr;
                        bool _coloured = false;

                    public:
                        SerialSinkFactory();
                        SerialSinkFactory(const SerialSinkFactory&);
                        SerialSinkFactory(SerialSinkFactory&&) noexcept ;

#ifdef ARDUINO
                        SerialSinkFactory& SetStream(Stream*);
#else
                        SerialSinkFactory& SetStream(std::ostream*);
#endif

                        SerialSinkFactory& SetColoured(bool);

                        SPIN::Log::Sinks::SerialSink Build();

                        SerialSinkFactory& operator=(const SerialSinkFactory&);
                        SerialSinkFactory& operator=(SerialSinkFactory&&) noexcept ;
                };
            }
        }
    }
}

#endif