import requests
import json
import sys
import os
sys.path.append(os.path.realpath('../../Twitter_cred/'))
import twitter_credentials

#endpoint = "https://api.twitter.com/1.1/tweets/search/30day/dev.json" 
endpoint = "https://api.twitter.com/1.1/search/tweets.json" 

headers = {"Authorization":"Bearer "+str(twitter_credentials.MYREALLYLONGTOKENBASEDONKEYS), "Content-Type": "application/json"}  

#data = '{"query":"(snow OR sleet OR hail OR (freezing rain)) has:images", "fromDate": "201802020000", "toDate": "201802240000"}'

data = '{"query":"GET snow"}'

print(headers)
print(data)
print()

#response = requests.post(endpoint,data=data,headers=headers).json()
response = requests.post("--request GET --url https://api.twitter.com/1.1/search/tweets.json?q=from%3Atwitterdev&result_type=mixed&count=2 --header authorization: OAuth oauth_consumer_key=consumer-key-for-app, oauth_nonce="generated-nonce", oauth_signature="generated-signature", 
                         #            oauth_signature_method="HMAC-SHA1", oauth_timestamp="generated-timestamp", 
                         #                oauth_token="access-token-for-authed-user", oauth_version="1.0"').json()

print(json.dumps(response, indent = 2))

#curl --request GET 
#    --url 'https://api.twitter.com/1.1/search/tweets.json?q=from%3Atwitterdev&result_type=mixed&count=2' 
#        --header 'authorization: OAuth oauth_consumer_key="consumer-key-for-app", 
#        oauth_nonce="generated-nonce", oauth_signature="generated-signature", 
#            oauth_signature_method="HMAC-SHA1", oauth_timestamp="generated-timestamp", 
#                oauth_token="access-token-for-authed-user", oauth_version="1.0"'
#
#$ twurl /1.1/search/tweets.json?q=from%3Atwitterdev&result_type=mixed&count=2

