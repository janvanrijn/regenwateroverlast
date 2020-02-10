import pandas as pd
import numpy as np

def pandafy_handpicked(file_name='../data/Excel sheet afstudeeropdracht Thijs Kroep.csv',save_name='../../pandafied_data/pandafied_handpicked.csv'):
    data = pd.read_csv(file_name)
    municipality=[]
    date = []
    medium = []
    platform = []
    link = []
    location = []
    radarX = []
    radarY = []
    
    for i in range(len(data['Gemeente'])):
        for j in range(1,7):
            try:
                if np.isnan(data['Vak'+str(j)][i]) or data['Vak'+str(j)][i] == 'nan':
                    append=False
                else:
                    append=True
            except:
                append=True
            if append:
                municipality.append(data['Gemeente'][i])
                date.append(data['Datum'][i][6:10]+data['Datum'][i][3:5]+data['Datum'][i][0:2])
                medium.append(data['Medium'][i])
                platform.append(data['Platform'][i])
                link.append(data['Link'][i])
                location.append(data['Locatie '+str(j)][i])
                radarY.append(str(int(data['Vak'+str(j)][i]))[0:3])
                radarX.append(str(int(data['Vak'+str(j)][i]))[3:6])
    dict = {}
    dict['municipality']=municipality
    dict['date']=date
    dict['medium']=medium
    dict['platform']=platform
    dict['link']=link
    dict['location']=location
    dict['radarX']=radarX
    dict['radarY']=radarY
    
    df = pd.DataFrame(dict)
    df.to_csv(save_name,index=False)
        
    
if __name__ == '__main__':
    pandafy_handpicked()
