from django.db import models


class Message(models.Model):
    message = models.CharField(max_length=500, blank=False, null=False)
    sent_by = models.ForeignKey('account.User', on_delete=models.DO_NOTHING, null=False, related_name='sent_msgs')
    sent_to = models.ForeignKey('account.User', on_delete=models.DO_NOTHING, null=False, related_name='rec_msgs')
    date = models.DateTimeField(auto_now_add=True)
