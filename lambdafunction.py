from __future__ import print_function

import json
import urllib
import boto3

print('Loading function')

s3 = boto3.client('s3')


def lambda_handler(event, context):

    #print("Received event: " + json.dumps(event, indent=2))

    # Get the object from the event and show its content type
    bucket = event['Records'][0]['s3']['bucket']['name']
    
    try:
        response = s3.get_object(Bucket=bucket, Key='currenttemp')
        #
        currenttemp = (response['Body'].read())
        print (currenttemp)
        responsedestemp = s3.get_object(Bucket=bucket, Key='destemp')
        destemp = (responsedestemp['Body'].read())
        print (destemp)
        if currenttemp < destemp:
            s3.put_object(Bucket=bucket, Key='heaterstate', Body='1', ACL='public-read')
        else:
            s3.put_object(Bucket=bucket, Key='heaterstate', Body='0', ACL='public-read')
        

    except Exception as e:
        print(e)
        print('Error getting object {} from bucket {}. Make sure they exist and your bucket is in the same region as this function.'.format(key, bucket))
        raise e
