# ====================================================================
#
# Licensed under the GNU General Public License v3.0;
# you may not use this file except in compliance with the License.
#
# ====================================================================

from actor_logger import logger


class Action:
    STATUS_RUNNING = 'Running'
    STATUS_COMPLETED = 'Completed'
    STATUS_FAILED = 'Failed'

    def __init__(self, action_name, action_paras, action_type):
        self.action_output_summary = None
        self.action_output = None
        self.status = None
        self.name = action_name
        self.paras = action_paras
        self.type = action_type

    def action_running(self, task_id):
        self.status = Action.STATUS_RUNNING
        message = {"type": "action running", "id": task_id}
        action_res_all = "\n" + "Action <" + self.name + "> is running..."
        message['results'] = action_res_all
        return message

    def get_action_para(self, key):
        if key in self.paras.keys():
            return self.paras[key]
        else:
            return None

    def action_completed(self, task_id, action_output_summary, action_output):
        self.status = Action.STATUS_COMPLETED
        self.action_output_summary = action_output_summary
        self.action_output = action_output
        message = {"type": "action completed", "id": task_id}
        action_res_all = "Action <" + self.name + "> completed!"
        message['results'] = action_res_all
        message['output summary'] = action_output_summary
        #message['output'] = action_output
        return message

    def action_notfound(self, task_id, action_output_summary):
        self.status = Action.STATUS_FAILED
        self.action_output_summary = action_output_summary
        message = {"type": "action failed", "id": task_id}
        action_res_all = "Action <" + self.name + "> failed!"
        message['results'] = action_res_all
        message['output summary'] = action_output_summary
        return message

    # def set_exec_done(self, action_output_summary, action_output):
    #     self.status = Action.STATUS_COMPLETED
    #     self.action_output_summary = action_output_summary
    #     self.action_output = action_output
    #
    # def set_exec_failed_notfound(self, action_output_summary):
    #     self.status = Action.STATUS_FAILED
    #     self.action_output_summary = action_output_summary

class Task:
    STATUS_NEW = 'New'
    STATUS_PENDING = 'Pending'
    STATUS_RUNNING = 'Running'
    STATUS_COMPLETED = 'Completed'
    STATUS_CANCELLED = 'Cancelled'
    STATUS_QUIT_ERRORS = 'Quit with errors'

    def __init__(self, message):
        # The message from the server (in json) should have the following format:
        # {'name': task_name, 'id': task_id, 'actions': [{'name': action_name, 'paras': action_paras, 'type': action_type, }, {}, ... ]
        self.id = message['id']  # an ID generated by the server, not included in the test script.
        self.name = message['test_name']
        self.message = message
        self.actions = self.parse_actions()
        self.current = -1  # the current action index in the actions list
        self.actions_status = [None] * len(self.actions)
        self.task_status = Task.STATUS_NEW   # Task status taken from 'New', 'Pending', 'Running', 'Completed', 'Cancelled', 'Quit with errors'

    def parse_actions(self):
        action_list = []
        actions_msg = self.message['actions']
        for item in actions_msg:
            #action = Action(item['name'], item['paras'], item['type'])
            action = Action(item['name'], item['paras'], None)
            action_list.append(action)

        return action_list

    def first_five_actions_str(self):
        size = len(self.actions) if len(self.actions) < 5 else 5
        str_five = ""
        for i in range(size):
            str_five = str_five + self.actions[i].name + "; "
        return str_five

    def next_action(self):
        self.current = self.current + 1
        if self.current >= len(self.actions):
            return None
        else:
            return self.actions[self.current]

    def task_running(self):
        self.task_status = Task.STATUS_RUNNING
        message = {"type": "task running", "id": self.id}
        action_res_all = "Test task is now running..."
        message['results'] = action_res_all
        return message

    def task_completed(self):
        self.task_status = Task.STATUS_COMPLETED
        # TO DO: generate a report summarizing the test results to the server
        message = {"type": "task completed", "id": self.id}
        action_res_all = "\n" + "Test task completed!"
        # action_res_all = []
        # for action in self.actions:
        #     action_res = {'name': action.name, 'status': action.status, 'description': action.action_output_summary}
        #     action_res_all.append(action_res)
        message['results'] = action_res_all
        return message
