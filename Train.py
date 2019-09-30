from __future__ import unicode_literals
from Railways import Parser

try:
    parser = Parser.Parser(conf = 'Config/Windows/Train.conf')
    '''
    create iSensor project
    '''
    parser.createDatabase('Config/Windows/Module.conf')
    parser.createModules()
    parser.convertFiles()
    parser.migrate()
    parser.createMainTest()
#     parser.createKeilUVProject()
    
    print('\t\tTrainFramework generate files successful')
except Exception as ex:
    print('\n\t\t!!!TrainFramework generate files ERROR!!!:\n\t\t',ex)     
    