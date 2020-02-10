import requests
from requests_oauthlib import OAuth1
import sys
import os
import json
sys.path.append(os.path.realpath('../../Twitter_cred/'))
import twitter_credentials
import csv

file_name = '../../Twitter_cred/full_arch_test.txt'

url = 'https://api.twitter.com/1.1/account/verify_credentials.json'
auth = OAuth1(twitter_credentials.CONSUMER_KEY, twitter_credentials.CONSUMER_SECRET, twitter_credentials.ACCESS_TOKEN, twitter_credentials.ACCESS_TOKEN_SECRET)
requests.get(url, auth=auth)

#endpoint = 'https://api.twitter.com/1.1/statuses/user_timeline.json'
#endpoint = 'https://api.twitter.com/1.1/search/tweets.json'
endpoint = 'https://api.twitter.com/1.1/tweets/search/fullarchive/testing.json'

#query = 'screen_name=realDonaldTrump&count=100'
#query = 'query=vla flip&count=100'
#query = "query=nat OR natte OR water OR wateroverlast OR regen OR storm OR blank OR bui OR overstroming OR hoosbui OR schade OR noodweer OR wolkbreuk OR waterschade"
query = "query=vla"
#query = "query=%23haiku+%23poetry&count=100"

do_query = False

if do_query:
    r = requests.get(endpoint+'?'+query, auth=auth)

    with open(file_name, 'a') as fp:
        fp.write(json.dumps(r.json())+'\n')

data = []
with open(file_name) as fp:
    for line in fp:
        data.append(json.loads(line))

for i in data:
    for j in i:
        print(j)
        print(i[j])
        print()
    print()
    print("----------------------------------")
    print("----------------------------------")
    print()
