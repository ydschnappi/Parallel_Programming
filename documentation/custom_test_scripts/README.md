## Custom Tests on the Submission Server

**Please Note: This feature is in BETA**

If you encounter issues, have suggestions, or would like to have additional software installed, please contact Vincent.

### Usage:

There is a special assignment available on the submission server [here](https://parprog.caps.in.tum.de/Submission/assignment/project-custom-test/view).
It allows you to submit a shell script which is run by the server so that you can run your own tests on our infrastructure. I know it's a bit of a hack but this is the best we can do for now. A sample script is provided in this directory. 
The places that you can edit are marked with *TODO*. You can then submit the script to the assignment so that it will be run. The output can be found in the `Run student submission` task. We have tested `perf` and can confirm that it should work.

Please note that your copy of the script includes a private SSH key that can be used to read and write to your repository, so be careful when sharing scripts.