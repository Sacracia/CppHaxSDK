from bs4 import BeautifulSoup
import requests

def clear_struct(struct, struct_name):
    struct = struct.replace('gint32', 'int32_t').replace('gunichar2', 'wchar_t').replace('MONO_ZERO_LEN_ARRAY', '1')
    if struct_name.startswith('_'):
        struct = struct.replace(struct_name, struct_name[1::])
    return struct

def find_struct(lines, struct, must, branch) -> str:
    header = 'struct ' + struct + ' {'
    begin = lines.find(header)
    if begin > 0:
        end = lines.index('};', begin)
        return clear_struct(lines[begin:end+2], struct) + '\n\n'
    
    header = '} ' + struct + ';'
    end = lines.find(header)
    if end > 0:
        begin = lines.rfind('typedef struct {', 0, end)
        return clear_struct(lines[begin:end+len(header)], struct) + '\n\n'
       
    if struct.startswith('_'):
        if must: 
            raise Exception(str(struct) + " not found in " + branch)
        return ''
    
    return find_struct(lines, '_' + struct, must, branch)

macro = {
    
}

branches = [
    ('unity-5.5',           'UNITY_5_5'),
    ('unity-5.6',           'UNITY_5_6'),
    ('unity-2017.1',        'UNITY_2017_1'),
    ('unity-2017.2',        'UNITY_2017_2'),
    ('unity-2017.3',        'UNITY_2017_3'),
    ('unity-2017.4',        'UNITY_2017_4'),
    ('unity-2018.1',        'UNITY_2018_1'),
    ('unity-2018.2',        'UNITY_2018_2'),
    ('unity-2018.3',        'UNITY_2018_3'),
    ('unity-2018.4',        'UNITY_2018_4'),
    ('unity-2022.1-mbe',    'UNITY_2022_1'),
    ('unity-2022.2-mbe',    'UNITY_2022_2'),
    ('unity-2022.3-mbe',    'UNITY_2022_3')
]

structs = {
    '/mono/metadata/object-internals.h' : [('_MonoException', True)],
    '/mono/metadata/class-internals.h' : [('_MonoClassField', True)]
}

file_begin = '''   
#if !defined(MONO_STRUCTS) && defined({})
#define MONO_STRUCTS

'''

file_end = '''
#endif
'''

for branch_t in branches:
    branch = branch_t[0]
    preview = ''
    f = open('mono/' + branch + '.h', 'w')
    f.write(file_begin.format(branch_t[1]))
    for link in structs:
        url = 'https://raw.githubusercontent.com/bonfirestudios/unity-mono/refs/heads/' + branch + link
        response = requests.get(url)
        if response.status_code != 200:
            raise Exception(url)
        soup = BeautifulSoup(response.text, 'html.parser')
        lines = soup.text
        
        for struct in structs[link]:            
            preview += find_struct(lines, struct[0], struct[1], branch)
            
    f.write(preview + file_end)
    f.close()