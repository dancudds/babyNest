# Lair

DIY IoT thermostate and humidity readings with optional controls.

For background see - Medium article

You will need to create your own Facebook App and Thingspeak channel.

You will also need an AWS account for the S3 bucket, Lambda and IAM permissions. The cost for this application is negliable (<$0.01/year).

##Set-up
Create an S3 bucket and a pre-signed URL. You can use various SDKs for this but I recommend Boto in Python
```
import boto
c = boto.connect_s3()
put_url = c.generate_url(315360000,'PUT',yourbucketname,'currenttemp', force_http=True, query_auth=True)
print (put_url)
```
Create a Facebook App for your domain (https://developer.facebook.com) and then set-up your AWS IAM role
```
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Principal": {
        "Federated": "graph.facebook.com"
      },
      "Action": "sts:AssumeRoleWithWebIdentity",
      "Condition": {
        "StringEquals": {
          "graph.facebook.com:app_id": "yourfacebookappid",
          "graph.facebook.com:id": "yourfacebookid"
        }
      }
    }
  ]
}
```



