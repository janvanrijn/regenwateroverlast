import numpy as np
import sys
import csv
import pandas as pd
import json
from datetime import datetime
from email.utils import parsedate_tz, mktime_tz
import pytz

def twit_time_to_standard(time_stamp):
    time_stamp = mktime_tz(parsedate_tz(time_stamp))
    dt = datetime.fromtimestamp(time_stamp, pytz.timezone('Europe/Amsterdam'))
    s = dt.strftime('%Y%m%d%H%M')
    return s

def pandafy_twitter(file_name='../../Twitter_cred/full_arch_2007-2020.txt',save_name='../../pandafied_data/pandafied_twitter_2007-2020.csv'):
    '''
        This function extracts the relevant information from a saved twitter query and puts it into a pandas dataframe.
    '''
    tweets = []
    with open(file_name) as fp:
        for line in fp:
            tweets.append(json.loads(line))
        
    text = []
    latlon = []
    time = []
    date = []
    for tweet in tweets:
        if not tweet['geo'] is None:
            text.append(tweet['text'])
            latlon.append((tweet['geo']['coordinates'][0],tweet['geo']['coordinates'][1]))
            time.append(twit_time_to_standard(tweet['created_at']))
            date.append(twit_time_to_standard(tweet['created_at'])[0:8])
    
    data_dict = {}
    data_dict['text'] = text
    data_dict['latlon'] = latlon
    data_dict['date'] = date
    data_dict['time'] = time
    
    pandas_data = pd.DataFrame(data_dict)
    pandas_data.to_csv(save_name,index=False)


if __name__ == '__main__':
    pandafy_twitter()
