package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.api;

import org.junit.internal.runners.statements.Fail;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.*;
import pt.ulisboa.tecnico.socialsoftware.tutor.auth.domain.AuthUser;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.FailedAnswerService;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.dto.DashboardDto;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.dto.FailedAnswerDto;

import java.security.Principal;
import java.util.List;

@RestController
public class FailedAnswerController {
    private static final Logger logger = LoggerFactory.getLogger(FailedAnswerController.class);

    @Autowired
    private FailedAnswerService failedAnswerService;

    FailedAnswerController(FailedAnswerService failedAnswerService) {
        this.failedAnswerService = failedAnswerService;
    }

    @GetMapping("/students/failedanswers/dashboard/{dashboardId}")
    @PreAuthorize("hasRole('ROLE_STUDENT') and hasPermission(#dashboardId, 'DASHBOARD.ACCESS')")
    public List<FailedAnswerDto> getFailedAnswers(@PathVariable int dashboardId){
        return this.failedAnswerService.getFailedAnswers(dashboardId);
    }

    @DeleteMapping("/students/failedanswers/failedanswer/{failedAnswerId}")
    @PreAuthorize("hasRole('ROLE_STUDENT') and hasPermission(#failedAnswerId, 'FAILED_ANSWER.ACCESS')")
    public void deleteFailedAnswers(@PathVariable int failedAnswerId){
        this.failedAnswerService.removeFailedAnswer(failedAnswerId);
    }

    @PutMapping("/students/updatefailedanswers/dashboard/{dashboardId}")
    @PreAuthorize("hasRole('ROLE_STUDENT') and hasPermission(#dashboardId, 'DASHBOARD.ACCESS')")
    public void updateFailedAnswers(@PathVariable int dashboardId,@RequestParam(required=false) String dateBefore, @RequestParam(required=false) String dateAfter){
        this.failedAnswerService.updateFailedAnswers(dashboardId, dateBefore, dateAfter);
    }
}