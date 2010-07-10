{
    'targets': [
        {
            'target_name': 'librgos',
            'type': '<(library)',
            'sources': [
                'src/rgos/Json.cpp',
                'src/rgos/JsonVisitor.cpp',
                'src/rgos/Serialize.cpp',
            ],
            'include_dirs': [
                'include',
            ],
            'dependencies': [
                ':check-deps',
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
            ],
            'direct_dependent_settings': {
                'include_dirs': [
                    'include',
                ],
            },
            'export_dependent_settings': [
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
            ],
        },
        {
            'target_name': 'librgos-tests',
            'type': 'executable',
            'sources': [
                'src/rgos/Json.test.cpp',
                'src/rgos/Serialize.test.cpp',
            ],
            'dependencies': [
                ':check-deps',
                ':librgos',
                '<(DEPTH)/ext/googlemock/googlemock.gyp:gmock_main',
            ],
        },
    ],
}
