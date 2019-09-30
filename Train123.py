from __future__ import unicode_literals
from Railways import Parser
# import os
# import shutil
# from jinja2 import Environment, FileSystemLoader
# from pyhocon import ConfigFactory
# import datetime
# import gzip
# import fileinput
# from css_html_js_minify import process_single_html_file, process_single_js_file, process_single_css_file, html_minify, js_minify, css_minify
# '''
# Create database string
# '''    
# print('Create database string start')
# module = ConfigFactory.parse_file('Module.conf')
# moduleString = ''
# moduleDbSt = 'typedef struct module{\n'
# for key in module['module']:
#     moduleString = moduleString + '\t"' +key + '{"'
#      
#     moduleDbSt = moduleDbSt + '\tstruct {\n'
#     moduleDbSt = moduleDbSt + '\t\tchar object['+str(len(str(key))+1)+']; //'+key+'=\n'
#         
#     size = len(module['module'][key])-1;
#     for idx, value in enumerate(module['module'][key]):
#         slen = len(str(value))+2+len(str(module['module'][key][value]))
#         if idx==size:
#             slen=slen+1
#         moduleString = moduleString + '\n\t\t"' + value + ':' + str(module['module'][key][value]) + ',"'
#         moduleDbSt = moduleDbSt + '\t\tchar '+value+'['+str(slen)+']; //'+value+':'+str(module['module'][key][value])+'\n';  
#     moduleString = moduleString[:-1] + '"\n\t"}"\n'
#     moduleDbSt = moduleDbSt + '\t}' +key + ';\n'    
# moduleDbSt = moduleDbSt + '}__attribute__((packed)) ModuleDb_st;'  
# print('Create database string start end\n')
#    
# '''
# replace text in file
# fName 
#     file name
# text 
#     text to replace
# to 
#     new string that replace
# '''
# def replaceInFile(fName,text,to):
#     for line in fileinput.FileInput(fName, inplace=True):
#         print(line.replace(text, to), end='') 
#              
#              
# '''
# load configuration file
# '''
# train = ConfigFactory.parse_file('Train.conf')
# env = Environment(loader=FileSystemLoader('.'))
#     
# train['db']['database'] = moduleString;
# train['db']['typedef'] = moduleDbSt;
#      
# '''
# parse jinja2 templates for 
#     modules
# with dictionary from 
#     configuration file
# '''
# print('parse jinja2 templates for modules start')
# for module in train['modules']:
#     for template in train[module]['template']:
#         try:
#             file = env.get_template(template['source'])
#             result = file.render(module = train[module], date = datetime.datetime.now().strftime("%Y-%m-%d"))
#             f = open(template['destination'], 'w')
#             f.write(result)
#             f.close()
#             print('\tfile',f.name,'generated from template to',template['destination'])
#         except Exception as ex:
#             print('\nOops! Template error:\n\t',ex)
#      
# print('parse jinja2 templates for modules end\n')  
# '''
# parse jinja2 template for 
#     test file 
# with dictionary from 
#     configuration file
# '''
# file = env.get_template(train['test'])     
# result = file.render(config = train, date = datetime.datetime.now().strftime("%Y-%m-%d"), name='Internet test')
# f = open('C:/CCpp/TrainTest/MainTest.c', 'w')
# f.write(result)
# f.close()
# print('file',f.name,'generated from template')
#      
# '''
# copy files from train-core to source folder
# '''
# print('\ncopy files from train-core to source folder start') 
# for path in train['migrate']['path']:
#     if not os.path.exists(path['dest']):
#         os.makedirs(path['dest'])
#     src = []
#     try:
#         src = os.listdir(path['source'])
#     except:
#         continue
#     for file in src:
#         file = os.path.join(path['source'],file)
#         if (os.path.isfile(file)):
#             shutil.copy(file, path['dest'])
#             print('\tcopy file',file,'to', path['dest'])
#      
# print('copy files from train-core to source folder end\n')     
# '''
# convert file to gzip and save as 
#     C bytes array
# create fileDescriptor structure with 
#     file name 
#     file size
# '''
# print('convert file to gzip and save as C bytes array start') 
# for file in train['files']['files']:
#     with open(file['source'], 'rb') as f_in:
#         with gzip.open(file['destination'], 'wb') as f_out:
#             shutil.copyfileobj(f_in, f_out)
#     byteString=''
#     fsize=0
#     with open(file['destination'], 'rb') as f:
#         byte = f.read(1)
#         byteString = (''.join('0x{:02x}'.format(x) for x in byte))
#         fsize=(os.fstat(f.fileno()).st_size)
#         while byte:
#             byte = f.read(1)
#             byteString = byteString + ', ' + (''.join('0x{:02x}'.format(x) for x in byte))
#         byteString = byteString[:-2]
#              
#         try:
#             ff = env.get_template(file['template'])
#             result = ff.render(module = file, date = datetime.datetime.now().strftime("%Y-%m-%d"))
#             f = open(file['destsource'], 'w')
#             f.write(result)
#             f.close()
#             print('\tfile',f.name,'generateg from template to',file['destsource'])
#             replaceInFile(file['destsource'],'@bytes@',byteString)
#             replaceInFile(file['destsource'],'@bytesSize@',str(fsize))
#         except Exception as ex:
#             print('\nOops! Template error:\n\t',ex)
#        
# print('convert file to gzip and save as C bytes array end\n')  
# 


parser = Parser.Parser()
'''
create iSensor project
'''
parser.createDatabase('iSensorDb.conf')
parser.createModules(modules='iSensor.stations')
parser.convertFiles(fileNames='iSensor.files.files')
parser.migrate(fileNames='iSensor.migrate.path')
parser.createMainTest(testTemplate='iSensor.test')


print('\n\n\t !TrainFramework generate files end!')     
