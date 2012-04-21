solution "ld23"
        language "C++"
        configurations { "Debug", "Release" }
        configuration "Debug"
                flags "Symbols"
        configuration "Release"
                flags "Optimize"
        project "tw"
                kind "WindowedApp"
                files "src/**.cpp"
                links { "sfml-system", "sfml-window", "sfml-graphics" }
