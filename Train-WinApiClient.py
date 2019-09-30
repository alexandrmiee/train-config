from __future__ import unicode_literals
#from Railways 
import Parser

try:
    parser = Parser.Parser(conf = 'Config/WinApiClient/WinApiClient.conf')

    parser.createDatabase('Config/WinApiClient/Module.conf')
    parser.createModules()
    parser.convertFiles()
    parser.migrate()
    parser.createMainTest()

    parser.createSublimeBuildGcc(projDir='C:\\CCpp\\WinApiClient\\')
    
    print('\t\tTrainFramework generate files successful')
except Exception as ex:
    print('\n\t\t!!!TrainFramework generate files ERROR!!!:\n\t\t',ex)     
    