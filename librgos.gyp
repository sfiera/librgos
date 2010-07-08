{
    'target_defaults': {
        'include_dirs': [
            'include',
        ],
    },
    'targets': [
        {
            'target_name': 'librgos',
            'type': '<(library)',
            'sources': [
                'src/rgos/Json.cpp',
                'src/rgos/JsonVisitor.cpp',
                'src/rgos/Serialize.cpp',
            ],
            'dependencies': [
                ':check-deps',
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
            ],
        },
    ],
}
