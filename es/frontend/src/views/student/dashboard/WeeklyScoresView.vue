<template>
  <div class="container">
    <v-card class="table">
      <v-row>
        <v-col>
          <h1>Weekly Scores</h1>
        </v-col>
        <v-col>
          <v-btn
            data-cy="refreshWeeklyScoresMenuButton"
            color="primary"
            dark
            @click="updateWeeklyScore"
          >
            Refresh
          </v-btn>
        </v-col>
      </v-row>

      <v-data-table
        :headers="headers"
        :items="weeklyScores"
        :sort-by="['week']"
        sort-desc
        :mobile-breakpoint="0"
        :items-per-page="15"
        :footer-props="{ itemsPerPageOptions: [10, 20, 40, 80] }"
        data-cy="dataTable"
      >
        <template v-slot:[`item.action`]="{ item }">
          <v-tooltip bottom>
            <template v-slot:activator="{ on }">
              <v-icon
                class="mr-2 action-button"
                v-on="on"
                data-cy="deleteWeeklyScoreButton"
                @click="deleteWeeklyScore(item)"
                color="red"
                >delete</v-icon
              >
            </template>
            <span>Delete Question</span>
          </v-tooltip>
        </template>
      </v-data-table>
    </v-card>
  </div>
</template>

<script lang="ts">
import { Component, Prop, Vue } from 'vue-property-decorator';
import RemoteServices from '@/services/RemoteServices';
import WeeklyScore from '@/models/dashboard/WeeklyScore';
import AnimatedNumber from '@/components/AnimatedNumber.vue';

@Component({
  components: {
    AnimatedNumber,
  },
})
export default class WeeklyScoresView extends Vue {
  @Prop({ default: 0 }) readonly dashboardId!: number;

  weeklyScores: WeeklyScore[] = [];

  headers: object = [
    {
      text: 'Actions',
      value: 'action',
      align: 'left',
      width: '5px',
      sortable: false,
    },
    {
      text: 'Number Answered',
      value: 'numberAnswered',
      width: '250px',
      align: 'center',
    },
    {
      text: 'Uniquely Answered',
      value: 'uniquelyAnswered',
      width: '250px',
      align: 'center',
    },
    {
      text: 'Percentage Correct',
      value: 'percentageCorrect',
      width: '250px',
      align: 'center',
    },
    {
      text: 'Week',
      value: 'week',
      width: '250px',
      align: 'center',
    },
    {
      text: 'Closed',
      value: 'closed',
      width: '250px',
      align: 'center',
    },
  ];

  async created() {
    await this.getWeeklyScore();
  }

  async getWeeklyScore() {
    await this.$store.dispatch('loading');
    try {
      this.weeklyScores = await RemoteServices.getWeeklyScore(this.dashboardId);
    } catch (error) {
      await this.$store.dispatch('error', error);
    }
    await this.$store.dispatch('clearLoading');
  }

  async updateWeeklyScore() {
    await this.$store.dispatch('loading');
    try {
      await RemoteServices.updateWeeklyScore(this.dashboardId);
    } catch (error) {
      await this.$store.dispatch('error', error);
    }
    await this.$store.dispatch('clearLoading');

    await this.getWeeklyScore();

    let date: any;
    date = (await RemoteServices.getUserDashboard()).lastCheckWeeklyScores;
    this.$emit('refresh', date);
  }

  async deleteWeeklyScore(weeklyScore: WeeklyScore) {
    if (weeklyScore.id) {
      try {
        await RemoteServices.removeWeeklyScore(weeklyScore.id);
        this.weeklyScores = this.weeklyScores.filter(
          (ws) => ws.id != weeklyScore.id
        );
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
  }
}
</script>

<style lang="scss" scoped>
.question-textarea {
  text-align: left;
  .CodeMirror,
  .CodeMirror-scroll {
    min-height: 200px !important;
  }
}
.option-textarea {
  text-align: left;
  .CodeMirror,
  .CodeMirror-scroll {
    min-height: 100px !important;
  }
}
</style>
