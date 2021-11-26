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

#include "PatchLibraries.h"
#include <launch/LaunchTask.h>
#include <minecraft/OneSixVersionFormat.h>
#include <FileSystem.h>
#include <QSaveFile>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

void PatchLibraries::executeTask()
{
    qDebug() << "Patching the instance to use system libraries.";

    auto component = m_components->getComponent("org.lwjgl3");
    bool isLwjgl3 = true;

    if (component == nullptr)
    {
        component = m_components->getComponent("org.lwjgl");

        if (component == nullptr)
        {
            emitFailed(tr("Failed to find LWJGL component for this Minecraft instance."));
            return;
        }

        isLwjgl3 = false;
    }

    qDebug() << "Checking if instance's LWJGL is customized.";

    if (component->customize())
    {
        qDebug() << "Instance's LWJGL is not customized, will patch.";

        bool patched = false;

        auto vfile = component->getVersionFile();

        if (isLwjgl3)
        {
#ifdef BGL_SYSTEM_LWJGL3_PATH
            qDebug() << "Instance uses LWJGL 3, applying appropiate patches.";

            vfile->version = "System";
            vfile->libraries = QList<LibraryPtr>();

            QString lwjgl3Dir(TOSTRING(BGL_SYSTEM_LWJGL3_PATH));

            QList<QString> libs = {
              "lwjgl-glfw", "lwjgl-openal", "lwjgl-opengl",
              "lwjgl-stb", "lwjgl-tinyfd", "lwjgl"
            };

            QList<QString> nativeLibs = {
              "lwjgl-opengl", "lwjgl-stb", "lwjgl-tinyfd", "lwjgl"
            };

            for (auto &libName : libs)
            {
                LibraryPtr lib(new Library());
                lib->setHint("local");
                lib->setPath(FS::PathCombine(lwjgl3Dir, libName + ".jar"));
                lib->setRawName(GradleSpecifier("org.lwjgl:" + libName + ":system"));
                vfile->libraries.append(lib);
            }

            for (auto &libName : nativeLibs)
            {
                LibraryPtr lib(new Library());
                lib->setHint("local");

#if defined(Q_OS_OPENBSD)
                lib->setPath(FS::PathCombine(lwjgl3Dir, libName + "-natives-openbsd.jar"));
#else
#error "Suffix for native LWJGL 3 libraries is not defined"
#endif

                lib->setRawName(GradleSpecifier("org.lwjgl:" + libName + ":system"));
                lib->setNative(true);
                vfile->libraries.append(lib);
            }

            patched = true;
#else
            qDebug() << "Instance uses LWJGL 3, system installed LWJGL 3 is not defined, skip patching.";
            qDebug() << "If the game doesn't launch because LWJGL 3 fails, contact the package maintainer.";
#endif
        } else {
#ifdef BGL_SYSTEM_LWJGL2_PATH
            qDebug() << "Instance uses LWJGL 2, applying appropiate patches.";

            vfile->version = "System";
            vfile->libraries = QList<LibraryPtr>();

            QString lwjglDir(TOSTRING(BGL_SYSTEM_LWJGL2_PATH));

            LibraryPtr jinput(new Library());
            jinput->setHint("local");
            jinput->setPath(FS::PathCombine(lwjglDir, "jinput.jar"));
            jinput->setRawName(GradleSpecifier("net.java.jinput:jinput:system"));
            vfile->libraries.append(jinput);

            LibraryPtr lwjgl(new Library());
            lwjgl->setHint("local");
            lwjgl->setPath(FS::PathCombine(lwjglDir, "lwjgl.jar"));
            lwjgl->setRawName(GradleSpecifier("org.lwjgl.lwjgl:lwjgl:system"));
            vfile->libraries.append(lwjgl);

            LibraryPtr lwjglUtil(new Library());
            lwjglUtil->setHint("local");
            lwjglUtil->setPath(FS::PathCombine(lwjglDir, "lwjgl_util.jar"));
            lwjglUtil->setRawName(GradleSpecifier("org.lwjgl.lwjgl:lwjgl_util:system"));
            vfile->libraries.append(lwjglUtil);

            patched = true;
#else
            qDebug() << "Instance uses LWJGL 3, system installed LWJGL 2 is not defined, skip patching.";
            qDebug() << "If the game doesn't launch because LWJGL 2 fails, contact the package maintainer.";
#endif
        }

        if (patched)
        {
            qDebug() << "Instance LWJGL is patched, writing patch file.";

            auto filename = component->getFilename();
            auto document = OneSixVersionFormat::versionFileToJson(vfile);

            QSaveFile jsonFile(filename);
            if(!jsonFile.open(QIODevice::WriteOnly))
            {
                emitFailed(tr("Failed to open patch file for writing."));
                return;
            }

            jsonFile.write(document.toJson());
            if (!jsonFile.commit())
            {
                emitFailed(tr("Failed to commit patch file."));
                return;
            }

            qDebug() << "Reloading instance's component list.";

            m_components->reload(m_mode);

            qDebug() << "Instance should now the system-wide LWJGL library!";
        }
        else
        {
            component->revert();
            qDebug() << "Instance LWJGL is not patched, skip writing patch file.";
        }
    }
    else
    {
        qDebug() << "Instance's LWJGL is customized/patched, leaving it as-is.";
    }

    emitSucceeded();
}

bool PatchLibraries::canAbort() const
{
    return false;
}

bool PatchLibraries::abort()
{
    return false;
}
