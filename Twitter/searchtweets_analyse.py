import sys
import os
import json
sys.path.append(os.path.realpath('../map_visualisation/'))
from plot_maps import *

file_name = '../../Twitter_cred/full_arch_2017.txt'

tweets = []
with open(file_name) as fp:
    for line in fp:
        tweets.append(json.loads(line))

point_and_box_plot_tweets(tweets)
exit()

counter = 0
for tweet in tweets:
    if not tweet['geo'] is None:
        #print(tweet['created_at'])
        #print(tweet['text'])
        for i in tweet:
            print(i)
            print(tweet[i])
            print()
        print()
        print("----------------------------------")
        print("----------------------------------")
        print()
        counter += 1

print(counter)
print(len(tweets))


#[print(tweet.all_text, end='\n\n') for tweet in tweets[0:10]]
#print()
#
#[print(tweet.created_at_datetime) for tweet in tweets[0:10]]
#print()
#
#[print(tweet.generator.get("name")) for tweet in tweets[0:10]]
#print()
