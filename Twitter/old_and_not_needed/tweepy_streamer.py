# YouTube Video: https://www.youtube.com/watch?v=wlnx-7cm4Gg
from tweepy.streaming import StreamListener
from tweepy import OAuthHandler
from tweepy import Stream
from plot_maps import *

import sys
import os
import json
sys.path.append(os.path.realpath('../../Twitter_cred/'))
import twitter_credentials
import re
import string
from datetime import datetime

# # # # TWITTER STREAMER # # # #
class TwitterStreamer():
    """
        Class for streaming and processing live tweets.
        """
    def __init__(self):
        pass
    
    def stream_tweets(self, fetched_tweets_filename, hash_tag_list):
        # This handles Twitter authetification and the connection to Twitter Streaming API
        listener = StdOutListener(fetched_tweets_filename)
        auth = OAuthHandler(twitter_credentials.CONSUMER_KEY, twitter_credentials.CONSUMER_SECRET)
        auth.set_access_token(twitter_credentials.ACCESS_TOKEN, twitter_credentials.ACCESS_TOKEN_SECRET)
        stream = Stream(auth, listener)
        
        box = [3.026594,51.071778,7.155537,53.699308]#Bounding box around the Netherlands
        # This line filter Twitter Streams to capture data by the keywords: 
        #stream.filter(track=hash_tag_list)
        stream.filter(locations=box)


# # # # TWITTER STREAM LISTENER # # # #
class StdOutListener(StreamListener):
    """
        This is a basic listener that just prints received tweets to stdout.
        """
    def __init__(self, fetched_tweets_filename):
        self.fetched_tweets_filename = fetched_tweets_filename
    
    def on_data(self, data):
        try:
            print(data)
            with open(self.fetched_tweets_filename, 'a') as tf:
                tf.write(data)
            return True
        except BaseException as e:
            print("Error on_data %s" % str(e))
        return True
    
    
    def on_error(self, status):
        print(status)

def load_stream(file_name):
    data = []
    with open(file_name) as f:
        for line in f:
            data.append(json.loads(line))
    return data

def filter_on_words(tweets,search_words):
    if len(search_words)==0:
        return tweets
    tweets_out = []
    for i in tweets:
        if any([s == j for j in re.sub('['+string.punctuation+']', '', i['text'].lower()).split() for s in search_words]):
            tweets_out.append(i)
    return tweets_out

if __name__ == '__main__':
    
    # Authenticate using config.py and connect to Twitter Streaming API.
    hash_tag_list = ["water"]
    fetched_tweets_filename = "../../Twitter_cred/tweets2.txt"
    
    search_words = ["nat", "natte", "water", "wateroverlast", "regen", "storm", "blank", "bui", "overstroming", "hoosbui", "schade", "noodweer", "wolkbreuk", "waterschade"]
    #search_words = ["wateroverlast", "storm", "bui", "overstroming", "hoosbui", "noodweer", "wolkbreuk", "waterschade"]
    #search_words = []
    
    if len(sys.argv) >= 2 and sys.argv[1] == 'stream':
        twitter_streamer = TwitterStreamer()
        twitter_streamer.stream_tweets(fetched_tweets_filename, hash_tag_list)
    elif len(sys.argv) >= 2 and sys.argv[1] == 'load':
        #load previouw stream and analyze
        previous_stream = load_stream(fetched_tweets_filename)
        
        time_stamps = [int(tweet['timestamp_ms'])/1000.0 for tweet in previous_stream]
        print("first tweet: " + str(datetime.fromtimestamp(time_stamps[0])))
        print("last tweet: " + str(datetime.fromtimestamp(time_stamps[-1])))
        total = len(previous_stream)
        previous_stream = filter_on_words(previous_stream,search_words)
        selection = len(previous_stream)
        
        print(str(selection) + " out of " + str(total) + " tweets selected.")
        print("That is " +str(selection*100.0/total) +"%")
        print()
        
        
        #box_plot_tweets(previous_stream)
        point_and_box_plot_tweets(previous_stream)
        #heatmap_tweets(previous_stream)
    else:
        print("usage: python3 tweepy_streamer.py <stream/load>")
