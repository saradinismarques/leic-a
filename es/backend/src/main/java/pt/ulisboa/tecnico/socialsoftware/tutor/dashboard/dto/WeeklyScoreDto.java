package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.WeeklyScore;
import pt.ulisboa.tecnico.socialsoftware.tutor.utils.DateHandler;

import java.io.Serializable;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.Objects;

public class WeeklyScoreDto implements Serializable {

    @JsonProperty("id")
    private Integer id;

    @JsonProperty("numberAnswered")
    private int numberAnswered;

    @JsonProperty("uniquelyAnswered")
    private int uniquelyAnswered;

    @JsonProperty("percentageCorrect")
    private int percentageCorrect;

    @JsonProperty("week")
    private LocalDate week;

    public WeeklyScoreDto(){
    }

    public WeeklyScoreDto(WeeklyScore weeklyScore){
        setId(weeklyScore.getId());
        setNumberAnswered(weeklyScore.getNumberAnswered());
        setUniquelyAnswered(weeklyScore.getUniquelyAnswered());
        setPercentageCorrect(weeklyScore.getPercentageCorrect());
        setWeek(weeklyScore.getWeek());
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public int getNumberAnswered() {
        return numberAnswered;
    }

    public void setNumberAnswered(int numberAnswered) {
        this.numberAnswered = numberAnswered;
    }

    public int getUniquelyAnswered() {
        return uniquelyAnswered;
    }

    public void setUniquelyAnswered(int uniquelyAnswered) {
        this.uniquelyAnswered = uniquelyAnswered;
    }

    public int getPercentageCorrect() {
        return percentageCorrect;
    }

    public void setPercentageCorrect(int percentageCorrect) {
        this.percentageCorrect = percentageCorrect;
    }

    public String getWeek() {
        return DateHandler.toISOString(week.atStartOfDay());
    }

    public void setWeek(LocalDate week) {
        this.week = week;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        WeeklyScoreDto that = (WeeklyScoreDto) o;
        return numberAnswered == that.numberAnswered &&
                uniquelyAnswered == that.uniquelyAnswered &&
                percentageCorrect == that.percentageCorrect &&
                id.equals(that.id) &&
                week.equals(that.week);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, numberAnswered, uniquelyAnswered, percentageCorrect, week);
    }

    @Override
    public String toString() {
        return "WeeklyScoreDto{" +
                "id=" + id +
                ", numberAnswered=" + numberAnswered +
                ", uniquelyAnswered=" + uniquelyAnswered +
                ", percentageCorrect=" + percentageCorrect +
                ", week=" + week +
                '}';
    }
}