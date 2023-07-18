<template>
  <v-container fluid>
    <v-data-table
        :headers="headers"
        :items="failedAnswers"
        :sort-by="['collected']"
        sort-desc
        :mobile-breakpoint="0"
        :items-per-page="15"
        :footer-props="{ itemsPerPageOptions: [15, 30, 50, 100] }"
    >
      <template v-slot:top>
        <v-card-title>
          <v-spacer></v-spacer>
          <h2>Failed Answers</h2>
          <v-spacer></v-spacer>
          <v-btn
              color="primary"
              dark
              data-cy="refreshFailedAnswerButton"
              @click="onFailedAnswersRefresh"
          >Refresh</v-btn>
        </v-card-title>
      </template>

      <template #item="{ item }">
        <tr>
          <td id="action">
            <v-tooltip top>
              <template v-slot:activator="{ on }">
                <v-icon
                    class="mr-2 action-button"
                    data-cy="showStudentViewDialog"
                    v-on="on"
                    @click="showStudentViewDialog(item.questionAnswerDto.question)"
                >school</v-icon>
              </template>
              <span>Student View</span>
            </v-tooltip>
            <v-tooltip top>
              <template v-slot:activator="{ on }">
                <v-icon
                    class="mr-2 action-button"
                    data-cy="deleteFailedAnswerButton"
                    v-on="on"
                    color="red"
                    @click="deleteFailedAnswer(item)"
                >delete</v-icon>
              </template>
              <span>Delete Failed Answer</span>
            </v-tooltip>
          </td>
          <td id="questionName">
            <v-layout justify-center>
              {{ item.questionName }}
            </v-layout>
          </td>
          <td id="answered">
            <v-layout justify-center>
              <v-spacer></v-spacer>
              {{ item.answered ? 'Yes' : 'No' }}
            </v-layout>
          </td>
          <td id="collected">
            <v-layout justify-center>
              {{ item.collected }}
            </v-layout>
          </td>
        </tr>
      </template>
    </v-data-table>
    <student-view-dialog
        v-if="statementQuestion && studentViewDialog"
        v-model="studentViewDialog"
        :statementQuestion="statementQuestion"
        v-on:close-show-question-dialog="onCloseStudentViewDialog"
    />
  </v-container>
</template>

<script lang="ts">
import {Component, Prop, Vue} from 'vue-property-decorator';
import RemoteServices from '@/services/RemoteServices';
import FailedAnswer from '@/models/dashboard/FailedAnswer';
import StudentViewDialog from '@/views/teacher/questions/StudentViewDialog.vue';
import Question from '@/models/management/Question';
import StatementQuestion from '@/models/statement/StatementQuestion'

@Component({
  components: {
    'student-view-dialog': StudentViewDialog,
  },
})
export default class FailedAnswersView extends Vue {
  @Prop({ type: Number, required: true }) readonly dashboardId!: number;
  @Prop({ type: String, required: true }) lastCheckFailedAnswers!: string;
  @Prop({ type: Boolean }) readOnly: boolean | undefined;

  failedAnswers: FailedAnswer[] = [];
  headers = FailedAnswer.failedAnswerHeader.slice();
  statementQuestion: StatementQuestion | null = null;
  studentViewDialog: boolean = false;

  async created() {
    await this.getFailedAnswerQuestions();
  }

  async getFailedAnswerQuestions() {
    await this.$store.dispatch('loading');
    try {
      this.failedAnswers = await RemoteServices.getFailedAnswers(this.dashboardId);
    } catch (error) {
      await this.$store.dispatch('error', error);
    }
    await this.$store.dispatch('clearLoading');
  }

  async updateFailedAnswers() {
    await this.$store.dispatch('loading');
    try {
      await RemoteServices.updateFailedAnswers(this.dashboardId);
    } catch (error) {
      await this.$store.dispatch('error', error);
    }

    let date = (await RemoteServices.getUserDashboard()).lastCheckFailedAnswers;
    this.$emit('refresh', date);
    await this.$store.dispatch('clearLoading');
  }

  async onFailedAnswersRefresh() {
    await this.updateFailedAnswers();
    await this.getFailedAnswerQuestions();
  }

  async showStudentViewDialog(question: Question) {
    if (question.id) {
      try {
        this.statementQuestion = await RemoteServices.getStatementQuestion(
            question.id
        );
        this.studentViewDialog = true;
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
  }

  onCloseStudentViewDialog() {
    this.statementQuestion = null;
    this.studentViewDialog = false;
  }

  async deleteFailedAnswer(
      toDeleteFailedAnswer: FailedAnswer
  ) {
    if (
        toDeleteFailedAnswer.id &&
        confirm('Are you sure you want to delete this failed answer?')
    ) {
      try {
        await RemoteServices.deleteFailedAnswer(
            toDeleteFailedAnswer.id
        );
        this.failedAnswers = this.failedAnswers.filter(
            (failedAnswer) =>
                failedAnswer.id != toDeleteFailedAnswer.id
        );
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
  }

}
</script>

