import json
import optparse
import sys

def _ParseOptions():
    parser = optparse.OptionParser()
    parser.add_option('-i',
                      '--input_file',
                      help='input version json5')
    parser.add_option('-o',
                      '--output_file',
                      help='output version file')
    options, _ = parser.parse_args()
    return options

def ReadHeader(options):
    defaultVersion = "4.1.6.1"
    with open(options.input_file, 'r') as f:
        data = json.load(f)
        if data is None:
            return defaultVersion
        if "app" in data and "versionName" in data["app"]:
            version = data["app"]["versionName"].split("sp")[0]
            return version
    return defaultVersion

def WriteHeader(options, version):
    header_macros = "ARKWEB_VERSION_H"
    with open(options.output_file, "w") as output_file:
        output_file.write('#ifndef %s\n' % header_macros)
        output_file.write('#define %s\n\n' % header_macros)
        output_file.write('\n#define ARKWEB_VERSION \"%s\"\n' % version)
        output_file.write('\n#endif  // %s\n' % header_macros)


if '__main__' == __name__:
    options = _ParseOptions()
    WriteHeader(options, ReadHeader(options))
    