# RIC Platform E2 Termination
Mirror of upstream Gerrit repo

*This is a fork of the ric-plt-e2 repository published by the O-RAN software community.*

## Deployment Instructions

### Pre-requisite: Local docker registry
To store docker images. You can create one using, (You will need "super user" permissions)

 `sudo docker run -d -p 5001:5000 --restart=always --name ric registry:2`
 
Now you can either push or pull images using,

`docker push localhost:5001/<image_name>:<image_tag>`  or  `docker pull localhost:5001/<image_name>:<image_tag>`
 
 ### Creating Docker image
The code in this repo needs to be packaged as a docker container. We make use of the existing Dockerfile in RIC-E2-TERMINATION to do this. Execute the following commands in the given order 
```
cd RIC-E2-TERMINATION
sudo docker build -f Dockerfile -t localhost:5001/ric-plt-e2:5.5.0 .
sudo docker push localhost:5001/ric-plt-e2:5.5.0
```
***TODO***: *Alternatively, you can just pull the image hosted on Github Packages (https://ghcr.io)*

### Deployment
That's it! Now, the image you just created can be deployed on your RIC (ric-plt) Kubernetes cluster. Modify the *e2term* section in the recipe file present in `dep/RECIPE_EXAMPLE/PLATFORM` to include your image,

<pre><code>
e2term:
  alpha:
    image:
      <b>registry: "localhost:5001"
      name: ric-plt-e2
      tag: 5.5.0</b>
    privilegedmode: false
    hostnetworkmode: false
    env:
      print: "1"
      messagecollectorfile: "/data/outgoing/"
    dataVolSize: 100Mi
    storageClassName: local-storage
    pizpub:
      enabled: false
</pre></code>
When the RIC platform is deployed, you will have the modified E2 Termination running on the Kubernetes cluster. The pod will be called `deployment-ricplt-e2term-alpha` and 3 services related to E2 Termination will be created:
- *service-ricplt-e2term-prometheus-alpha* : Communicates with the *VES-prometheus Adapter (VESPA)* pod to exchange data which will be sent to the SMO.
- *service-ricplt-e2term-rmr-alpha* : RMR service that manages exchange of messages between E2 Termination other components in the near-real time RIC.
- *service-ricplt-e2term-sctp-alpha* : Accepts SCTP connections from RAN and exchanges E2 messages with the RAN. Note that this service is configured as a *NodePort* (accepts connections external to the cluster) while the other two are configured as *ClusterIP* (Networking only within the cluster). 

## Commands related to E2 Termination
- View E2 Termination logs : `kubectl logs -f -n ricplt -l app=ricplt-e2term-alpha`
- View E2 Manager Logs : `kubectl logs -f -n ricplt -l app=ricplt-e2mgr`
- Get the IP *service-ricplt-e2term-sctp-alpha* : `kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`




