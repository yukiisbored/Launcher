/* Copyright 2021 Blockgame Contributors
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

#include <launch/LaunchStep.h>
#include <minecraft/PackProfile.h>
#include <net/Mode.h>

class PatchLibraries: public LaunchStep
{
    Q_OBJECT
public:
    explicit PatchLibraries(LaunchTask *parent, Net::Mode mode, std::shared_ptr<PackProfile> components): LaunchStep(parent), m_mode(mode)
    {
        m_components = components;
    };
    virtual ~PatchLibraries(){};

    virtual void executeTask();
    virtual bool canAbort() const;
    virtual bool abort();
private:
    std::shared_ptr<PackProfile> m_components;
    Net::Mode m_mode = Net::Mode::Offline;
};
