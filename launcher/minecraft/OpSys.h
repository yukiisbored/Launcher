/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <QString>
enum OpSys
{
    Os_Windows,
    Os_Linux,
    Os_OSX,
    Os_OpenBSD,
    Os_Other
};

OpSys OpSys_fromString(QString);
QString OpSys_toString(OpSys);

#if defined(Q_OS_WIN32)
#define currentSystem Os_Windows
#elif defined(Q_OS_MAC)
#define currentSystem Os_OSX
#elif defined(Q_OS_LINUX)
#define currentSystem Os_Linux
#elif defined(Q_OS_OPENBSD)
#define currentSystem Os_OpenBSD
#else
#define currentSystem Os_Other
#endif
