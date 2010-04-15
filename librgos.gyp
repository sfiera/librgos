{
    'target_defaults': {
        'include_dirs': [
            'include',
            '<(DEPTH)/ext/libsfz/include',
        ],
        'xcode_settings': {
            'GCC_TREAT_WARNINGS_AS_ERRORS': 'YES',
            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
            'SDKROOT': 'macosx10.4',
            'GCC_VERSION': '4.0',
            'ARCHS': 'ppc x86_64 i386',
            'WARNING_CFLAGS': [
                '-Wall',
                '-Wendif-labels',
            ],
        },
    },
    'targets': [
        {
            'target_name': 'check-deps',
            'type': 'none',
            'actions': [
                {
                    'action_name': 'check-deps',
                    'inputs': [ ],
                    'outputs': [ ],
                    'action': [
                        './scripts/check-deps.sh',
                        '<(DEPTH)',
                    ],
                },
            ],
        },
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