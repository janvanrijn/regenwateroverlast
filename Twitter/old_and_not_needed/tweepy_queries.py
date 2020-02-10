import tweepy
import sys
import os
import json
sys.path.append(os.path.realpath('../../Twitter_cred/'))
import twitter_credentials

auth = tweepy.OAuthHandler(twitter_credentials.CONSUMER_KEY, twitter_credentials.CONSUMER_SECRET)
auth.set_access_token(twitter_credentials.ACCESS_TOKEN, twitter_credentials.ACCESS_TOKEN_SECRET)

api = tweepy.API(auth)
rate_limit = api.rate_limit_status()

#for i in rate_limit['resources']['sandbox']['/sandbox/account_activity/webhooks/:id/subscriptions']:
#    print(i)

#query = 'nat OR natte OR water OR wateroverlast OR regen OR storm OR blank OR bui OR overstroming OR hoosbui OR schade OR noodweer OR wolkbreuk OR waterschade'
query = 'sinterklaas -piet'
max_tweets = 10
searched_tweets = [status for status in tweepy.Cursor(api.search, q=query).items(max_tweets)]

for i in searched_tweets:
    json_str = i._json
    print(json_str['text'])
    print()

#class API([auth_handler=None][, host='api.twitter.com'][, search_host='search.twitter.com'][, cache=None][, api_root='/1'][, search_root=''][, retry_count=0][, retry_delay=0][, retry_errors=None][, timeout=60][, parser=ModelParser][, compression=False][, wait_on_rate_limit=False][, wait_on_rate_limit_notify=False][, proxy=None])

#API.rate_limit_status()
#API.search(q[, lang][, locale][, rpp][, page][, since_id][, geocode][, show_user])
