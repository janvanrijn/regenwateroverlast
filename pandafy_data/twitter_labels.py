import pandas as pd

if __name__ == '__main__':
    print(0)
    rain = pd.read_csv('../../pandafied_data/pandafied_rain.csv')
    radar = pd.read_csv('../../pandafied_data/pandafied_radar_coord.csv')
    tweets_XY = pd.read_csv('../../pandafied_data/pandafied_twitter_XY.csv')
    print(1)
    tweets_in_radar = pd.merge(tweets_XY, radar, on=('radarX','radarY'), how='inner')
    print(2)
    rain_per_day = rain.groupby(['date','radarX','radarY'])['rain'].sum().reset_index(name='rainsum')
    print(3)
    rain_select = rain_per_day[(rain_per_day.rainsum >= 0) & (rain_per_day.date >= 20170000) & (rain_per_day.date < 20180000)]
    print(4)
    radar_select = pd.merge(rain_select, radar, on=('radarX','radarY'), how='inner')
    print(5)
    radar_tweets = pd.merge(radar_select, tweets_XY, on=('radarX','radarY','date'), how='inner')
    print(6)
    
    #threshold = 30000
    threshold = 10000
    step = 1000
    
    
    search_terms = ["nat" ,"natte", "water", "wateroverlast", "regen", "storm", "blank", "bui", "overstroming", "hoosbui", "schade", "noodweer", "wolkbreuk", "waterschade"]
    while True:
        high_low = [[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]]
        for i in range(len(radar_tweets['text'])):
            for j in range(len(search_terms)):
                if (radar_tweets['rainsum'][i] >= threshold) and (search_terms[j] in radar_tweets['text'][i]):
                    high_low[j][0] += 1
                elif (radar_tweets['rainsum'][i] < threshold) and (search_terms[j] in radar_tweets['text'][i]):
                    high_low[j][1] += 1
        high_sum = sum([i[0] for i in high_low])
        low_sum = sum([i[1] for i in high_low])
        print(threshold,high_sum,low_sum)
        if high_sum <= low_sum:
            break
        threshold += step

    for i in range(len(search_terms)):
        print(search_terms[i]+", high rain: " + str(high_low[i][0]) + " low rain: " + str(high_low[i][1]))
            
