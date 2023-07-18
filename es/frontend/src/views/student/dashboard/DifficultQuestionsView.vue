<template>
    <v-container fluid>
        <v-card class="table">
        <v-row>
            <v-col>
            <h1>Difficult Questions</h1>
            </v-col>
            <v-col>
            <v-btn data-cy="refreshDifficultQuestionsMenuButton" color="primary" dark @click="updateDifficultQuestions">
                Refresh
            </v-btn>
            </v-col>
        </v-row>

        <v-data-table
            :headers="headers"
            :items="difficultQuestions"
            :sort-by="['percentage']"
            sort-desc
            :mobile-breakpoint="0"
            :items-per-page="15"
            :footer-props="{ itemsPerPageOptions: [10, 20, 40, 80] }"
        >
            <template v-slot:[`item.action`]="{ item }">
            <v-tooltip bottom>
                <template v-slot:activator="{ on }">
                <v-icon
                    class="mr-2 action-button"
                    v-on="on"
                    data-cy="showStudentViewDialogButton"
                    @click="showStudentViewDialog(item.questionDto)"
                    >school</v-icon
                >
                </template>
                <span>Student View</span>
            </v-tooltip>
            <v-tooltip bottom>
                <template v-slot:activator="{ on }">
                <v-icon
                    class="mr-2 action-button"
                    v-on="on"
                    data-cy="deleteDifficultQuestionButton"
                    @click="deleteDifficultQuestion(item)"
                    color="red"
                    >delete</v-icon
                >
                </template>
                <span>Delete Question</span>
            </v-tooltip>
            </template>
        </v-data-table>
        <student-view-dialog
            v-if="statementQuestion && studentViewDialog"
            v-model="studentViewDialog"
            :statementQuestion="statementQuestion"
            v-on:close-show-question-dialog="onCloseStudentViewDialog"
        />
        </v-card>
    </v-container>
</template>

<script lang="ts">
import { Component, Vue, Prop } from 'vue-property-decorator';
import RemoteServices from '@/services/RemoteServices';
import AnimatedNumber from '@/components/AnimatedNumber.vue';
import Question from '@/models/management/Question';
import DifficultQuestion from '@/models/dashboard/DifficultQuestion';
import StatementQuestion from '@/models/statement/StatementQuestion';
import StudentViewDialog from '@/views/teacher/questions/StudentViewDialog.vue';

@Component({
    components: { 
    AnimatedNumber,
    'student-view-dialog': StudentViewDialog,
    },
})
export default class DifficultQuestionsView extends Vue {
    @Prop({ default: 0 }) readonly dashboardId!: number;

    difficultQuestions: DifficultQuestion[] = [];
    statementQuestion: StatementQuestion | null = null;
    studentViewDialog: boolean = false;

    headers: object = [
        {
        text: 'Actions',
        value: 'action',
        align: 'left',
        width: '5px',
        sortable: false,
        },
        {
        text: 'Question',
        value: 'questionDto.title',
        width: '70%',
        align: 'left',
        sortable: false,
        },
        {
        text: 'Percentage',
        value: 'percentage',
        width: '250px',
        align: 'center',
        },
    ];

    async created() {
        this.getDifficultQuestions();
    }

    async getDifficultQuestions() {
        await this.$store.dispatch('loading');
        try {
            this.difficultQuestions = await RemoteServices.getDifficultQuestions(this.dashboardId);
        } catch (error) {
            await this.$store.dispatch('error', error);
        }
        await this.$store.dispatch('clearLoading');
    }

    async updateDifficultQuestions() {
        await this.$store.dispatch('loading');
        try {
            await RemoteServices.updateDifficultQuestions(this.dashboardId);
        } catch (error) {
            await this.$store.dispatch('error', error);
        }
        await this.$store.dispatch('clearLoading');

        this.getDifficultQuestions();

        let date = (await RemoteServices.getUserDashboard()).lastCheckDifficultQuestions;
        this.$emit('refresh', date);
    }

    async deleteDifficultQuestion(difficultQuestion: DifficultQuestion) {
        await this.$store.dispatch('loading');
        try {
            await RemoteServices.removeDifficultQuestion(difficultQuestion.id);
            this.difficultQuestions = await RemoteServices.getDifficultQuestions(this.dashboardId);
        } catch (error) {
            await this.$store.dispatch('error', error);
        }
        await this.$store.dispatch('clearLoading');
    }

    async showStudentViewDialog(question: Question) {
        if (question.id) {
            try {
                this.statementQuestion = await RemoteServices.getStatementQuestion(question.id);
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
}
</script>

<style lang="scss" scoped>
.stats-container {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  justify-content: center;
  align-items: stretch;
  align-content: center;
  height: 100%;

  .items {
    background-color: rgba(255, 255, 255, 0.75);
    color: #1976d2;
    border-radius: 5px;
    flex-basis: 25%;
    margin: 20px;
    cursor: pointer;
    transition: all 0.6s;
  }
}

.icon-wrapper,
.project-name {
  display: flex;
  align-items: center;
  justify-content: center;
}

.icon-wrapper {
  font-size: 100px;
  transform: translateY(0px);
  transition: all 0.6s;
}

.icon-wrapper {
  align-self: end;
}

.project-name {
  align-self: start;
}
.project-name p {
  font-size: 24px;
  font-weight: bold;
  letter-spacing: 2px;
  transform: translateY(0px);
  transition: all 0.5s;
}

.items:hover {
  border: 3px solid black;

  & .project-name p {
    transform: translateY(-10px);
  }
  & .icon-wrapper i {
    transform: translateY(5px);
  }
}
</style>
