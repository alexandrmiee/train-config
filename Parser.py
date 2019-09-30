from __future__ import unicode_literals
import os
import shutil
from jinja2 import Environment, FileSystemLoader
from pyhocon import ConfigFactory
import datetime
import gzip
import fileinput
from os import walk
# from css_html_js_minify import process_single_html_file, process_single_js_file, process_single_css_file, html_minify, js_minify, css_minify

class Parser:
    """class for engine jinja2 templates"""
    
    def __init__(
            self,
            env = Environment(loader=FileSystemLoader('.')),
            conf = None
        ):
        self.env = env
        
        if conf is None:
            self.train = ConfigFactory.parse_file('Train.conf')
        else:
            self.train = ConfigFactory.parse_file(conf)

##############################################################################################
    
    def replaceInFile(self,fName,text,to):
        '''
        replace text in file
        fName 
            file name
        text 
            text to replace
        to 
            new string that replace
        '''
        for line in fileinput.FileInput(fName, inplace=True):
            print(line.replace(text, to), end='') 
            
##############################################################################################

    def createDatabase(self,config=None):
        '''
        Create database string
        '''   
        print('Create database string start')        
        if config is None:
            module = ConfigFactory.parse_file('Module.conf')
        else:
            module = ConfigFactory.parse_file(config)
        moduleString = ''
        moduleDbSt = 'typedef struct module{\n'
        iTables = 0
        iRows = 0
        for key in module['module']:
            moduleString = moduleString + '\t"' +key + '{"'
            
            moduleDbSt = moduleDbSt + '\tstruct {\n'
            moduleDbSt = moduleDbSt + '\t\tchar object['+str(len(str(key))+1)+']; //'+key+'=\n'
            iTables = iTables + 1
               
            size = len(module['module'][key])-1;
            for idx, value in enumerate(module['module'][key]):
                keyLen = len(str(value))+1                          #key:
                valueLen = len(str(module['module'][key][value]))+1 #value,
                if idx==size:
                    valueLen=valueLen+1
                moduleString = moduleString + '\n\t\t"' + value + ':' + str(module['module'][key][value]) + ',"'
                moduleDbSt = moduleDbSt + '\t\tchar '+value+'['+str(keyLen)+']; //'+value+':\n';
                moduleDbSt = moduleDbSt + '\t\tchar '+value+'Val['+str(valueLen)+']; //'+str(module['module'][key][value])+',\n';
                iRows = iRows + 1
                  
            moduleString = moduleString[:-1] + '"\n\t"}"\n'
            moduleDbSt = moduleDbSt + '\t}__attribute__((packed)) ' +key + ';\n'    
        moduleDbSt = moduleDbSt + '}__attribute__((packed)) ModuleDb_st;'  
        self.moduleString = moduleString;
        self.moduleDbSt = moduleDbSt;     
        self.iTables = iTables   
        self.iRows = iRows   
        print('Create database string start end\n')
 
##############################################################################################

    def createModules(
            self,
            config = None,
            modules='modules'
        ):
        '''
        parse jinja2 templates for 
            modules
        with dictionary from 
            configuration file
        '''
        if config is None:
            config = self.train
        else: 
            config = ConfigFactory().parse_file(config)
            
        print('parse jinja2 templates for modules start')
        for module in config[modules]:
            print('\t',str(module))
            if str(module) == 'database':
                config[module]['database'] = self.moduleString
                config[module]['typedef'] = self.moduleDbSt                
                config[module]['tables'] = self.iTables                
                config[module]['rows'] = self.iRows                
            for template in config[module]['template']:
                try:
                    file = self.env.get_template(template['source'])
                    result = file.render(module = config[module], date = datetime.datetime.now().strftime("%Y-%m-%d"),modules=config[modules])
                    
                    directory = os.path.dirname(template['destination'])
                    if not os.path.exists(directory):
                        os.makedirs(directory)
                
                    f = open(template['destination'], 'w')
                    f.write(result)
                    f.close()
                    print('\t\tfile',f.name,'generated from template to',template['destination'])
                except Exception as ex:
                    print('\nOops! Template error:\n\t',ex)        
                    raise Exception('in file '+template['destination'])    
        print('parse jinja2 templates for modules end\n')
    
############################################################################################## 
    
    def migrate(self,fileNames=None):
        '''
        copy files from train-core to source folder
        '''
        if fileNames is None:
            files=self.train['migrate']['path']
        else:
            files=self.train[fileNames]
        print('\ncopy files from train-core to source folder start') 
        for path in files:
            if not os.path.exists(path['dest']):
                os.makedirs(path['dest'])
            src = []
            try:
                src = os.listdir(path['source'])
            except:
                continue
            for file in src:
                file = os.path.join(path['source'],file)
                if (os.path.isfile(file)):
                    shutil.copy(file, path['dest'])
                    print('\tcopy file',file,'to', path['dest'])
            
        print('copy files from train-core to source folder end\n') 

##############################################################################################      
    def convertFiles(self,fileNames=None):
        '''
        convert file to gzip and save as 
            C bytes array
        create fileDescriptor structure with 
            file name 
            file size
        '''
        if fileNames is None:
            files=self.train['files']['files']
        else:
            files=self.train[fileNames]
            
        print('convert file to gzip and save as C bytes array start') 
        for file in files:
            with open(file['source'], 'rb') as f_in:
                with gzip.open(file['destination'], 'wb') as f_out:
                    shutil.copyfileobj(f_in, f_out)
            byteString=''
            fsize=0
            with open(file['destination'], 'rb') as f:
                byte = f.read(1)
                byteString = (''.join('0x{:02x}'.format(x) for x in byte))
                fsize=(os.fstat(f.fileno()).st_size)
                while byte:
                    byte = f.read(1)
                    byteString = byteString + ', ' + (''.join('0x{:02x}'.format(x) for x in byte))
                byteString = byteString[:-2]
                    
                try:
                    ff = self.env.get_template(file['template'])
                    result = ff.render(module = file, date = datetime.datetime.now().strftime("%Y-%m-%d"))
                    f = open(file['destsource'], 'w')
                    f.write(result)
                    f.close()
                    print('\tfile',f.name,'generateg from template to',file['destsource'])
                    self.replaceInFile(file['destsource'],'@bytes@',byteString)
                    self.replaceInFile(file['destsource'],'@bytesSize@',str(fsize))
                except Exception as ex:
                    print('\nOops! Template error:\n\t',ex)
                    raise Exception('in file '+file['destsource']) 
              
        print('convert file to gzip and save as C bytes array end\n') 
        
##############################################################################################      
    def createMainTest(self,testTemplate=None):
        '''
        parse jinja2 template for 
            test file 
        with dictionary from 
            configuration file
        '''
        if testTemplate is None:
            test = self.train['test']
        else:
            test = self.train[testTemplate]
        file = self.env.get_template(test['source'])     
        result = file.render(config = self.train, date = datetime.datetime.now().strftime("%Y-%m-%d"), name=str(test['name']))
        
        if not os.path.exists(test['destination']):
                os.makedirs(test['destination'])
                
        f = open(str(test['destination'])+'main.c', 'w')
        f.write(result)
        f.close()
        print('file',f.name,'generated from template')
        
##############################################################################################    
    def createSublimeBuildGcc(
        self,
        projDir,
        result='C:/Users/home/AppData/Roaming/Sublime Text 3/Packages/User/gcc.sublime-build'
    ):
        '''
        parse jinja2 templates for 
            gcc
        '''
        f = open('C:/CCpp/WinApiClient/gcc.bat', 'w')
        gcc_I = ''
        gcc_exe = 'gcc -o test.exe '
        for subdir, dirs, files in os.walk(projDir):
            files = [ fi for fi in files if fi.endswith(".h") ]
            for file in files:
                gcc_I = gcc_I + ' -I' + '"'+subdir+'"'
        
        for subdir, dirs, files in os.walk(projDir):
            files = [ fi for fi in files if fi.endswith(".c") ]
            for file in files:
                gcc_O = 'gcc '+ gcc_I + ' -O0 -g3 -Wall -c -fmessage-length=0 -o ' + os.path.splitext(file)[0]+'.o '+os.path.join(subdir,file)+'\n'
                f.write(gcc_O)
                
                gcc_exe = gcc_exe + os.path.splitext(file)[0]+'.o '

        gcc_exe = gcc_exe + ' -lws2_32 \n'   
        f.write(gcc_exe)

        f.close()

##############################################################################################
    def createKeilUVProject(
        self,
        config = None,
        modules = 'modules',
        migrate = 'migrate.path',
        keil = 'keil'
    ):
        '''
        parse jinja2 templates for 
            Keil
        with dictionary from 
            configuration file
        '''
        if config is None:
            config = self.train
        else: 
            config = ConfigFactory().parse_file(config)
            
        print('parse jinja2 templates for Keil start')

        config['folders'] = []
        for module in config[modules]:
            files = []
            folder = ''
            item = dict([])
            for template in config[module]['template']:
                file = dict([])
                folder = os.path.dirname(template['source'])
                template['folder'] = os.path.dirname(template['source'])
                template['fileName'] = os.path.basename(template['destination'])
                file.update({'fileName':os.path.basename(template['destination'])})
                filename, file_extension = os.path.splitext(template['destination'])
                if file_extension=='.c':
                    file.update({'fileType':1})
                elif file_extension=='.h':
                    file.update({'fileType':5})
                files.append(file)
            item.update({'folder':folder})
            item.update({'files':files})
            config['folders'].append(item)
        
        for module in config[migrate]:
            files = []
            folder = os.path.dirname(module['source'])
            item = dict([])
            for template in os.listdir(module['dest']):
                for (dirpath, dirnames, filenames) in walk(module['source']):
                    for f in filenames:
                        file = dict([])
                        file.update({'fileName':f})
                        filename, file_extension = os.path.splitext(f)
                        if file_extension=='.c':
                            file.update({'fileType':1})
                        elif file_extension=='.h':
                            file.update({'fileType':5})
                        files.append(file)
            item.update({'folder':folder})
            item.update({'files':files})
            config['folders'].append(item)
        
        
        files = []
        folder = 'Main'
        item = dict([])            
        file = dict([])
        file.update({'fileName':'main.c'})
        file.update({'fileType':1})
        files.append(file)
        item.update({'folder':folder})
        item.update({'files':files})
        config['folders'].append(item)
            
        for template in config[keil]['template']:
            try:            
                file = self.env.get_template(template['source'])
                
                result = file.render(modules = config['folders'], date = datetime.datetime.now().strftime("%Y-%m-%d"))
                directory = os.path.dirname(template['destination'])
                if not os.path.exists(directory):
                    os.makedirs(directory)
                        
                f = open(template['destination'], 'w')
                f.write(result)
                f.close()
                print('\t\tfile',f.name,'generated from template to',template['destination'])
            except Exception as ex:
                print('\nOops! Template error:\n\t',ex)
                raise Exception('in file '+ template['destination']) 
        print('parse jinja2 templates for Keil end\n')
        
    
