package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.api;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.WeeklyScoreService;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.dto.WeeklyScoreDto;

import java.util.List;

@RestController
public class WeeklyScoreController {
    private static final Logger logger = LoggerFactory.getLogger(DashboardController.class);

    @Autowired
    private WeeklyScoreService weeklyScoreService;

    WeeklyScoreController(WeeklyScoreService weeklyScoreService) {
        this.weeklyScoreService = weeklyScoreService;
    }

    @GetMapping("/weeklyScores/dashboard/{dashboardId}")
    @PreAuthorize("hasRole('ROLE_STUDENT') and hasPermission(#dashboardId, 'DASHBOARD.ACCESS')")
    public List<WeeklyScoreDto> getWeeklyScore(@PathVariable int dashboardId) {
        return weeklyScoreService.getWeeklyScores(dashboardId);
    }

    @DeleteMapping("/students/weeklyScores/weeklyscore/{weeklyScoreId}")
    @PreAuthorize("hasRole('ROLE_STUDENT') and hasPermission(#weeklyScoreId, 'WEEKLY_SCORE.ACCESS')")
    public void removeWeeklyScore(@PathVariable int weeklyScoreId) {
        weeklyScoreService.removeWeeklyScore(weeklyScoreId);
    }

    @PutMapping("/students/updateweeklyscore/dashboard/{dashboardId}")
    @PreAuthorize("hasRole('ROLE_STUDENT') and hasPermission(#dashboardId, 'DASHBOARD.ACCESS')")
    public void updateWeeklyScore(@PathVariable int dashboardId){
        weeklyScoreService.updateWeeklyScore(dashboardId);
    }
}