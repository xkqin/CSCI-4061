NOTE:(I aksed this question to professor, he said I have to make a note to explain the situation) for the waitPipelineTermination function we check WEXITSTATUS(status) is not
equal to 0 and kill the pipline, but for the grep command if will return 1 if there is no line to find, when waitPipelineTermination catch the status it will make this status as a worning
status and kill the pipline. The professor didn't know how to fix this situation and asked me to make a note to TAs.
